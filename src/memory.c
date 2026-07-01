#include "types.h"
#include "log.h"
#include "memory.h"

extern uint8_t	__heap_start[];
extern uint8_t	__heap_end[];

#define HEAP_BLOCK_SIZE	32
#define HEAP_START		__heap_start
#define HEAP_END		__heap_end
#define HEAP_SIZE		((uint32_t)HEAP_END - (uint32_t)HEAP_START)

typedef struct block
{
    uint8_t			free;
	uint32_t		size;
    struct block	*next;
}	block_t;

static block_t	*heap = (block_t *)HEAP_START;

int	heap_init(void)
{
	heap->size = HEAP_SIZE - sizeof(block_t);
    heap->free = 1;
    heap->next = 0;
	return 0;
}

void	*malloc(uint32_t size)
{
	size = (size + 7) & ~7; // align to 8 bytes

	block_t *current = heap;
	while (current)
	{
		if (current->free && current->size >= size)
		{
			// check if we can split the block
			if (current->size >= size + sizeof(block_t) + 8)
			{
				block_t *new_block = (block_t *)((uint8_t *)current + sizeof(block_t) + size);
				new_block->size = current->size - size - sizeof(block_t);
				new_block->free = 1;
				new_block->next = current->next;

				current->next = new_block;
				current->size = size;
			}
			current->free = 0;
			#if TRACK_MEMORY_USAGE
			uint64_t	used = 0;
			uint64_t	total = 0;
			get_memory_usage(&used, &total);
			float percentage = (100.0f * (float)used) / (float)total;
			int integer_part = (int)percentage;
			int decimal_part = (int)((percentage - integer_part) * 100.0f + 0.5f);
			log("MEMORY USAGE: %llu/%llu [%d.%d%%]\n", LOG_INFO | LOG_INDENT, used, total, integer_part, decimal_part);
			#endif
			return (void *)((uint8_t *)current + sizeof(block_t));
		}
		current = current->next;
	}
	return 0;
}

int	free(void *ptr)
{
	if (!ptr)
		return 1;
	block_t *block = (block_t *)((uint8_t *)ptr - sizeof(block_t));
	block->free = 1;
	return 0;
}

void	*memcpy(void *dst, void *src, uint32_t size)
{
	for (uint32_t i=0; i<size; i++)
		((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
	return dst;
}

void	*memset(void *dst, int c, uint32_t size)
{
	for (uint32_t i=0; i<size; i++)
		((unsigned char *)dst)[i] = (unsigned char)c;
	return dst;
}

void	get_memory_usage(uint64_t *used, uint64_t *total)
{
	block_t	*block = heap;
	*used = 0;
	*total = 0;
	while (block)
	{
		if (!block->free)
			*used += block->size;
		*total += block->size;
		block = block->next;
	}
}