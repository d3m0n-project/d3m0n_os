#ifndef MMU_H
#define MMU_H

#include "types.h"

typedef struct s_section_info t_section_info;

typedef struct	s_address_space
{
	uint32_t	*l1;
}	t_address_space;


void			mmu_write_ttbr0(uint32_t);
void			mmu_invalidate_tlb();
void			mmu_barrier();
uint32_t		*mmu_kernel_table(void);

void			mmu_setup(void);
void			mmu_switch_table(uint32_t *table);
int				mmu_map_user_range(t_address_space *space, void *address, size_t size, int writable, int executable);
t_address_space	address_space_create(void *image_ptr, size_t size, t_section_info *);

#endif