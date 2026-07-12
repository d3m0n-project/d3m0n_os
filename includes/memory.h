#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#ifndef TRACK_MEMORY_USAGE
  #define TRACK_MEMORY_USAGE 0
#endif

extern uint8_t	__heap_start[];
extern uint8_t	__heap_end[];

#define HEAP_BLOCK_SIZE 32
#define HEAP_START      __heap_start
#define HEAP_END		    __heap_end
#define HEAP_SIZE		    ((uint32_t)HEAP_END - (uint32_t)HEAP_START)

void	*malloc(uint32_t size);
int		free(void *ptr);
int		heap_init(void);

void	*memcpy(void *dst, void *src, uint32_t size);
void	*memset(void *dst, int c, uint32_t size);

void	get_memory_usage(uint64_t *used, uint64_t *total);

#endif