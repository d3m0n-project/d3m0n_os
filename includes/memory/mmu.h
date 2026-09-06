#ifndef MMU_H
#define MMU_H

#include "types.h"
#include "proc/elf.h"

typedef struct	s_address_space
{
	uint32_t	*l1;
}	t_address_space;


void			mmu_write_ttbr0(uint32_t);
void			mmu_invalidate_tlb();
void			mmu_barrier();
uint32_t		*mmu_kernel_table(void);

void			mmu_setup(void);
t_address_space	address_space_create(void *image_ptr, size_t size, t_section_info *);

#endif