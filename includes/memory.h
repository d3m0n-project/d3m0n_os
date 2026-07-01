#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#ifndef TRACK_MEMORY_USAGE
  #define TRACK_MEMORY_USAGE 0
#endif

void	*malloc(uint32_t size);
int		free(void *ptr);
int		heap_init(void);

void	*memcpy(void *dst, void *src, uint32_t size);
void	*memset(void *dst, int c, uint32_t size);

void	get_memory_usage(uint64_t *used, uint64_t *total);

#endif