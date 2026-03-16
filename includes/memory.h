#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void	*malloc(uint32_t size);
int		free(void *ptr);
int		heap_init(void);
void	*memcpy(void *dst, void *src, uint32_t size);
void	*memset(void *dst, int c, uint32_t size);

#endif