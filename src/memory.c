#include "types.h"
#include "log.h"

#define HEAP_BLOCK_SIZE	32
#define HEAP_START		0x01000000
#define HEAP_SIZE		(16 * 1024 * 1024)

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