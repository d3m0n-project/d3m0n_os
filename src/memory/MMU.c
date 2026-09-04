#include "types.h"
#include "memory/mmu.h"

#define SECTION		(1u << 1)

/*
 * Section descriptor:
 * [31:20] Physical section base
 * [19:12] implementation-defined / ignored
 * [11:10] AP
 * [9:5]   domain
 * [4]     1 = section
 * [3:2]   C/B cache attributes
 * [1:0]   10 = section descriptor
 */

#define AP_RW				(3u << 10)
#define DOMAIN0				(0u << 5)

#define CACHEABLE			(1u << 3)
#define BUFFERABLE			(1u << 2)

#define MMU_TABLE_ENTRIES	4096
#define SECTION_SIZE		0x00100000u


static uint32_t		mmu_table[MMU_TABLE_ENTRIES] __attribute__((section(".mmu_table"), aligned(MMU_TABLE_ENTRIES * 4)));


static inline void	cp15_write_ttbr0(uint32_t value)
{
	asm volatile (
		"mcr p15, 0, %0, c2, c0, 0"
		:
		: "r"(value)
		: "memory"
	);
}

static inline void	cp15_write_dacr(uint32_t value)
{
	asm volatile (
		"mcr p15, 0, %0, c3, c0, 0"
		:
		: "r"(value)
		: "memory"
	);
}

static inline uint32_t	cp15_read_sctlr(void)
{
	uint32_t value;
	asm volatile (
		"mrc p15, 0, %0, c1, c0, 0"
		: "=r"(value)
		:
		: "memory"
	);

	return value;
}

static inline void	cp15_write_sctlr(uint32_t value)
{
	asm volatile (
		"mcr p15, 0, %0, c1, c0, 0"
		:
		: "r"(value)
		: "memory"
	);
}


static inline void	tlb_invalidate_all(void)
{
	asm volatile (
		"mcr p15, 0, %0, c8, c7, 0"
		:
		: "r"(0)
		: "memory"
	);
}

static inline void	mmu_barrier(void)
{
	asm volatile (
		"mcr p15, 0, r0, c7, c10, 4\n"  // DSB
		"mcr p15, 0, r0, c7, c5, 4\n"   // ISB
		:
		:
		: "r0", "memory"
	);
}


void	mmu_setup(void)
{
	// all unmaped
	for (unsigned i = 0; i < MMU_TABLE_ENTRIES; i++)
		mmu_table[i] = 0;


	/*
	 * Identity-map the first 512 MiB.
	 *
	 * VA 0x00000000 -> PA 0x00000000
	 * VA 0x00100000 -> PA 0x00100000
	 * ...
	 * VA 0x1FF00000 -> PA 0x1FF00000
	 *
	 * Pi Zero has 512 MiB RAM.
	 */
	for (unsigned i = 0; i < 512; i++) {
		mmu_table[i] =
			(i << 20) |
			AP_RW |
			DOMAIN0 |
			CACHEABLE |
			BUFFERABLE |
			SECTION;
	}


	/*
	 * BCM2835 peripherals:
	 *
	 * 0x20000000 - 0x20FFFFFF
	 *
	 * Map them identity-wise but DON'T cache them.
	 */
	for (unsigned i = 0x200; i < 0x210; i++) {
		mmu_table[i] =
			(i << 20) |
			AP_RW |
			DOMAIN0 |
			SECTION;
	}


	cp15_write_dacr(1u);


	// set table addr
	cp15_write_ttbr0((uint32_t)mmu_table);
	tlb_invalidate_all();


	uint32_t sctlr = cp15_read_sctlr();
	sctlr |= (1u << 0);   // MMU enable
	sctlr |= (1u << 2);   // data cache enable
	sctlr |= (1u << 12);  // instruction cache enable

	mmu_barrier();
	cp15_write_sctlr(sctlr);
	mmu_barrier();
}