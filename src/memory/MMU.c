#include "types.h"
#include "memory/mmu.h"
#include  "memory/memory.h"
#include "proc/elf.h"
#include "proc/proc.h"

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

#define MMU_PAGE_SIZE		0x1000u
#define L1_INDEX(address)	((address) >> 20)
#define L2_INDEX(address)	(((address) >> 12) & 0xFFu)



static uint32_t	mmu_table[L1_ENTRIES] __attribute__((aligned(L1_ALIGNMENT), section(".mmu_table")));

extern	void	*stack_top;


void	mmu_write_ttbr0(uint32_t value)
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


void	mmu_invalidate_tlb(void)
{
	uint32_t zero = 0;
	asm volatile(
		"mcr p15, 0, %0, c8, c7, 0"
		:
		: "r"(zero)
		: "memory"
	);
}


void	mmu_barrier(void)
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

static uint32_t	*alloc_l2_table(void)
{
	uintptr_t	allocation;

	allocation = (uintptr_t)ft_calloc(L2_ENTRIES + (L2_ALIGNMENT / sizeof(uint32_t)), sizeof(uint32_t));
	if (!allocation)
		return 0;
	allocation = (allocation + L2_ALIGNMENT - 1) & ~(uintptr_t)(L2_ALIGNMENT - 1);
	return (uint32_t *)allocation;
}

static uint32_t	*alloc_l1_table(void)
{
	uintptr_t	allocation;

	allocation = (uintptr_t)ft_calloc(L1_ENTRIES + (L1_ALIGNMENT / sizeof(uint32_t)), sizeof(uint32_t));
	if (!allocation)
		return 0;
	allocation = (allocation + L1_ALIGNMENT - 1) & ~(uintptr_t)(L1_ALIGNMENT - 1);
	return (uint32_t *)allocation;
}

static uint32_t	*ensure_l2(uint32_t *l1, uint32_t address)
{
	uint32_t	*table;
	uint32_t	index = L1_INDEX(address);

	if ((l1[index] & 3u) == COARSE_PAGE_TABLE)
		return (uint32_t *)(uintptr_t)(l1[index] & 0xFFFFFC00u);
	table = alloc_l2_table();
	if (!table)
		return 0;
	for (unsigned i = 0; i < L2_ENTRIES; ++i)
		table[i] = small_page_descriptor((index << 20) | (i << 12), AP_PRIV_RW_USER_NONE, 1, 1, 1);
	l1[index] = coarse_descriptor((uint32_t)(uintptr_t)table);
	return table;
}

static int	map_range(uint32_t *l1, uintptr_t vaddr, uintptr_t paddr, size_t size, uint32_t ap, uint32_t xn)
{
	uintptr_t	vend = vaddr + size;
	uintptr_t	pend = paddr + size;

	if (vend < vaddr || pend < paddr)
		return 1;
	for (uintptr_t va = vaddr & ~((uintptr_t)MMU_PAGE_SIZE - 1), pa = paddr & ~((uintptr_t)MMU_PAGE_SIZE - 1);
	     va < vend;
	     va += MMU_PAGE_SIZE, pa += MMU_PAGE_SIZE)
	{
		uint32_t *l2 = ensure_l2(l1, (uint32_t)va);
		uint32_t index = L2_INDEX((uint32_t)va);
		uint32_t old = l2 ? l2[index] : 0;
		uint32_t old_ap = old & (L2_APX | (3u << L2_AP_SHIFT));
		uint32_t default_ap = L2_AP_PRIV_RW_USER_NONE;
		uint32_t merged_ap = ap;
		uint32_t merged_xn = xn;

		if (!l2)
			return 1;
		/* Existing user mappings can share a page at section boundaries.
		 * Keep the page writable/executable if any section needs that access. */
		if ((old & 3u) && old_ap != default_ap)
		{
			if (old_ap == L2_AP_PRIV_RW_USER_RW || ap == L2_AP_PRIV_RW_USER_RW)
				merged_ap = L2_AP_PRIV_RW_USER_RW;
			else
				merged_ap = L2_AP_PRIV_RO_USER_RO;
			merged_xn = ((old & L2_XN) != 0) && (xn != 0);
		}
		l2[index] = small_page_descriptor((uint32_t)pa, merged_ap, 1, 1, merged_xn);
	}
	return 0;
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
			AP_PRIV_RW_USER_NONE,
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
			AP_PRIV_RW_USER_NONE,
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
	sctlr |= (1u << 23);	  // extended page table format (APX/XN)

	sctlr &= ~(1u << 2);   // data cache disabled
	sctlr &= ~(1u << 12);  // instruction cache disabled

	mmu_write_sctlr(sctlr);
	mmu_barrier();
}

void	mmu_switch_table(uint32_t *table)
{
	if (!table)
		return;
	mmu_barrier();
	mmu_invalidate_tlb();
	mmu_barrier();
	mmu_write_ttbr0((uint32_t)(uintptr_t)table);
	mmu_barrier();
	mmu_invalidate_tlb();
	mmu_barrier();
}

int	mmu_map_user_range(t_address_space *space, void *address, size_t size, int writable, int executable)
{
	uint32_t ap = writable ? L2_AP_PRIV_RW_USER_RW : L2_AP_PRIV_RO_USER_RO;
	uintptr_t addr = (uintptr_t)address;

	if (!space || !space->l1)
		return 1;
	// For user stacks, use identity mapping (virtual address = physical address)
	return map_range(space->l1, addr, addr, size, ap, executable ? 0 : 1);
}

int	mmu_map_kernel_range(t_address_space *space, void *address, size_t size)
{
	uintptr_t addr = (uintptr_t)address;

	if (!space || !space->l1)
		return 1;
	// For kernel stacks, use identity mapping (virtual address = physical address)
	return map_range(space->l1, addr, addr, size, L2_AP_PRIV_RW_USER_NONE, 1);
}

uint32_t	*mmu_kernel_table(void)
{
	return mmu_table;
}

t_address_space	address_space_create(void *image_ptr, size_t size, uint32_t vaddr_base, t_section_info *sections)
{
	uint32_t	*proc_mmu_table = alloc_l1_table();
	uintptr_t	image = (uintptr_t)image_ptr;
	if (!proc_mmu_table)
		return (t_address_space){0};
	for (unsigned i = 0; i < 528; ++i)
		proc_mmu_table[i] = mmu_table[i];
	
	// Check if we have any allocated sections
	int has_allocated_sections = 0;
	if (sections != NULL)
	{
		for (t_section_info *s = sections; s->size != 0; ++s)
		{
			if (s->flags & SHF_ALLOC)
			{
				has_allocated_sections = 1;
				log("MMU: found allocated section: %s at offset 0x%x size 0x%x flags 0x%x\n", 0, 
					s->name ? s->name : "(null)", s->start_offset, s->size, s->flags);
				break;
			}
		}
	}
	
	if (has_allocated_sections)
	{
		// Map only the allocated sections with their proper permissions
		for (t_section_info *s = sections; s->size != 0; ++s)
		{
			if (!(s->flags & SHF_ALLOC))
				continue;
			uint32_t ap = (s->flags & SHF_WRITE) ? L2_AP_PRIV_RW_USER_RW : L2_AP_PRIV_RO_USER_RO;
			uint32_t xn = (s->flags & SHF_EXECINSTR) ? 0 : 1;
			log("MMU: mapping section %s vaddr=0x%x paddr=0x%x size=0x%x ap=%d xn=%d\n", 0,
				s->name ? s->name : "(null)", vaddr_base + s->start_offset, (uint32_t)(image + s->start_offset), 
				s->size, ap, xn);
			// Map section at (vaddr_base + start_offset) to (image + start_offset)
			if (map_range(proc_mmu_table, (uintptr_t)(vaddr_base + s->start_offset), 
			              image + s->start_offset, s->size, ap, xn))
				return (t_address_space){0};
		}
		/* The ELF heap follows the image and has reserved physical backing. */
		if (size > USER_HEAP_RESERVED + 8)
		{
			size_t image_span = size - USER_HEAP_RESERVED - 8;
			if (map_range(proc_mmu_table, (uintptr_t)(vaddr_base + image_span),
			              image + image_span, size - image_span,
			              L2_AP_PRIV_RW_USER_RW, 1))
				return (t_address_space){0};
		}
	}
	else
	{
		// No allocated sections or no section info available, map the entire image with RW permissions
		log("MMU: mapping entire image vaddr=0x%x paddr=0x%x size=0x%x\n", 0, vaddr_base, (uint32_t)image, size);
		if (map_range(proc_mmu_table, (uintptr_t)vaddr_base, image, size, L2_AP_PRIV_RW_USER_RW, 1))
			return (t_address_space){0};
	}

	return (t_address_space){.l1 = proc_mmu_table};
}
