#include "types.h"
#include "memory/mmu.h"
#include  "memory/memory.h"

#define L1_ENTRIES				4096
#define L1_ALIGNMENT			0x4000

#define L2_ENTRIES				256
#define L2_ALIGNMENT			0x400

#define SECTION					0x2
#define COARSE_PAGE_TABLE		0x1
#define SMALL_PAGE				0x2

#define AP_PRIV_RW_USER_NONE	(1u << 10)
#define AP_PRIV_RW_USER_RW		(3u << 10)
#define AP_PRIV_RO_USER_RO		((1u << 15) | (2u << 10))
#define AP_FULL_ACCESS			(3u << 10)
#define DOMAIN0					(0u << 5)

#define L2_APX					(1u << 9)
#define L2_AP_SHIFT				4
#define L2_XN					(1u << 0)

#define L2_AP_PRIV_RW_USER_NONE	(1u << L2_AP_SHIFT)
#define L2_AP_PRIV_RW_USER_RW	(3u << L2_AP_SHIFT)
#define L2_AP_PRIV_RO_USER_RO	(L2_APX | (2u << L2_AP_SHIFT))

#define CACHEABLE		 		(1u << 3)
#define BUFFERABLE				(1u << 2)

#define XN						(1u << 4)



static uint32_t	mmu_table[L1_ENTRIES] __attribute__((aligned(L1_ALIGNMENT), section(".mmu_table")));


static inline void	mmu_write_ttbr0(uint32_t value)
{
	asm volatile(
		"mcr p15, 0, %0, c2, c0, 0"
		:
		: "r"(value)
		: "memory"
	);
}


static inline void	mmu_write_dacr(uint32_t value)
{
	asm volatile(
		"mcr p15, 0, %0, c3, c0, 0"
		:
		: "r"(value)
		: "memory"
	);
}


static inline uint32_t	mmu_read_sctlr(void)
{
	uint32_t value;
	asm volatile(
		"mrc p15, 0, %0, c1, c0, 0"
		: "=r"(value)
		:
		: "memory"
	);
	return value;
}


static inline void	mmu_write_sctlr(uint32_t value)
{
	asm volatile(
		"mcr p15, 0, %0, c1, c0, 0"
		:
		: "r"(value)
		: "memory"
	);
}


static inline void	mmu_invalidate_tlb(void)
{
	uint32_t zero = 0;
	asm volatile(
		"mcr p15, 0, %0, c8, c7, 0"
		:
		: "r"(zero)
		: "memory"
	);
}


static inline void	mmu_barrier(void)
{
	uint32_t zero = 0;
	asm volatile(
		"mcr p15, 0, %0, c7, c10, 4\n"
		"mcr p15, 0, %0, c7, c5, 4\n"
		:
		: "r"(zero)
		: "memory"
	);
}

static inline uint32_t	coarse_descriptor(uint32_t l2_address)
{
	uint32_t descriptor;
	descriptor = l2_address & 0xFFFFFC00u;
	descriptor |= COARSE_PAGE_TABLE;
	descriptor |= DOMAIN0;
	return descriptor;
}


static inline uint32_t	section_descriptor(uint32_t physical_address, uint32_t ap, uint32_t cacheable, uint32_t bufferable, uint32_t xn)
{
	uint32_t descriptor = physical_address & 0xFFF00000u;

	descriptor |= SECTION;
	descriptor |= ap;
	descriptor |= DOMAIN0;

	if (cacheable)
		descriptor |= CACHEABLE;

	if (bufferable)
		descriptor |= BUFFERABLE;

	if (xn)
		descriptor |= XN;

	return descriptor;
}

static inline uint32_t	small_page_descriptor(uint32_t physical_address, uint32_t ap, uint32_t cacheable, uint32_t bufferable, uint32_t xn)
{
	uint32_t descriptor = physical_address & 0xFFFFF000u;
	descriptor |= SMALL_PAGE;
	descriptor |= ap;

	if (cacheable)
		descriptor |= (1u << 3);

	if (bufferable)
		descriptor |= (1u << 2);

	if (xn)
		descriptor |= L2_XN;
	return descriptor;
}


void	mmu_setup(void)
{
	// all unmaped
	for (unsigned i = 0; i < L1_ENTRIES; ++i)
		mmu_table[i] = 0;


	// all 512Mb ram
	for (unsigned i = 0; i < 512; ++i)
	{
		uint32_t address = i << 20;
		mmu_table[i] = section_descriptor(
			address,
			AP_FULL_ACCESS,
			1,	  // cacheable
			1,	  // bufferable
			0	  // executable
		);
	}


	// peripherals addresses 0x20000000 - 0x20FFFFFF
	for (unsigned i = 512; i < 528; ++i)
	{
		uint32_t address = i << 20;
		mmu_table[i] = section_descriptor(
			address,
			AP_FULL_ACCESS,
			0,	  // not cacheable
			0,	  // not bufferable
			1	  // no execute
		);
	}

	mmu_write_dacr(1);


	// set table ptr
	mmu_write_ttbr0((uint32_t)mmu_table);


	mmu_invalidate_tlb(); // cleanup
	mmu_barrier();

	uint32_t sctlr = mmu_read_sctlr();
	sctlr |= (1u << 0);	   // MMU enabled

	sctlr &= ~(1u << 2);   // data cache disabled
	sctlr &= ~(1u << 12);  // instruction cache disabled

	mmu_write_sctlr(sctlr);
	mmu_barrier();
}

uint32_t	*mmu_kernel_table(void)
{
	return mmu_table;
}

t_address_space	address_space_create(void *image_ptr, size_t size, t_section_info *sections)
{
	uint32_t	*proc_mmu_table = ft_calloc(L1_ENTRIES, sizeof(uint32_t));
	if (!proc_mmu_table)
		return (t_address_space){0};

	uintptr_t	start;
	uintptr_t	end;

	uintptr_t	first;
	uintptr_t	last;
	for (t_section_info *s = sections; s->size != 0; ++s)
	{
		// ignore because not loaded
		if (!(s->flags & SHF_ALLOC))
			continue;

		start = image_ptr + s->start_offset;
		end = start + s->size;
		first = start >> 20;
		last = (end - 1) >> 20;
		for (unsigned i = first; i <= last; ++i)
		{
			uint32_t physical = i << 20;

			uint32_t ap = AP_PRIV_RO_USER_RO;
			uint32_t xn = 1;
			if (s->flags & SHF_WRITE)
				ap = AP_PRIV_RW_USER_RW;

			if (s->flags & SHF_EXECINSTR)
				xn = 0;

			proc_mmu_table[i] = section_descriptor(
				physical,
				ap,
				1,	  // cacheable
				1,	  // bufferable
				xn
			);
		}
	}

	return (t_address_space){.l1 = proc_mmu_table};
}