#include "sys.h"
#include "memory.h"

#define MALLOC_ALIGN		8
#define MALLOC_MIN_SIZE		16
#define MALLOC_MAGIC		0x4D414C4C

typedef struct s_malloc_block
{
	uint32_t			size;
	uint8_t				free;
	uint8_t				pad[3];
	struct s_malloc_block	*next;
}	t_malloc_block;

typedef struct s_heap_header
{
	uint32_t			magic;
	t_malloc_block		*free_list;
}	t_heap_header;

static uint32_t	align_up(uint32_t value, uint32_t align)
{
	return (value + (align - 1)) & ~(align - 1);
}

static void	coalesce_blocks(t_malloc_block *block)
{
	t_malloc_block	*next;

	while (block && block->free)
	{
		next = block->next;
		if (!next || !next->free || (char *)next != (char *)block + sizeof(t_malloc_block) + block->size)
			break;

		block->size += sizeof(t_malloc_block) + next->size;
		block->next = next->next;
	}
}

void	*malloc(size_t size)
{
	t_heap_header	*hdr;
	t_malloc_block	*block;
	t_malloc_block	*best;
	t_malloc_block	*split;
	uint32_t		aligned;
	int				base;

	aligned = (uint32_t)size;
	if (aligned < MALLOC_MIN_SIZE)
		aligned = MALLOC_MIN_SIZE;
	aligned = align_up(aligned, MALLOC_ALIGN);

	base = sbrk(0);
	if (base == (int)-1)
		return 0;

	hdr = (t_heap_header *)base;

	if (hdr->magic != MALLOC_MAGIC)
	{
		if (sbrk(USER_HEAP_RESERVED) == (int)-1)
			return 0;

		hdr->magic = MALLOC_MAGIC;
		hdr->free_list = (t_malloc_block *)((char *)hdr + sizeof(t_heap_header));

		hdr->free_list->size = USER_HEAP_RESERVED - sizeof(t_heap_header) - sizeof(t_malloc_block);
		hdr->free_list->free = 1;
		hdr->free_list->next = 0;
	}

	block = hdr->free_list;
	best = 0;
	while (block)
	{
		if (block->free && block->size >= aligned)
		{
			best = block;
			break;
		}
		block = block->next;
	}

	if (!best)
		return 0;

	if (best->size >= aligned + sizeof(t_malloc_block) + MALLOC_MIN_SIZE)
	{
		split = (t_malloc_block *)((char *)best + sizeof(t_malloc_block) + aligned);
		split->size = best->size - aligned - sizeof(t_malloc_block);
		split->free = 1;
		split->next = best->next;
		best->next = split;
		best->size = aligned;
	}

	best->free = 0;
	return (void *)((char *)best + sizeof(t_malloc_block));
}

void	free(void *ptr)
{
	t_heap_header	*hdr;
	t_malloc_block	*block;
	t_malloc_block	*prev;
	t_malloc_block	*cur;
	int				base;

	if (!ptr)
		return;

	base = sbrk(0);
	if (base == (int)-1)
		return;

	hdr = (t_heap_header *)base;
	if (hdr->magic != MALLOC_MAGIC)
		return;

	block = (t_malloc_block *)((char *)ptr - sizeof(t_malloc_block));

	if ((char *)block < (char *)base + sizeof(t_heap_header) ||
		(char *)block + sizeof(t_malloc_block) > (char *)base + USER_HEAP_RESERVED)
		return;

	if (block->free)
		return;

	block->free = 1;
	prev = 0;
	cur = hdr->free_list;
	while (cur && cur < block)
	{
		prev = cur;
		cur = cur->next;
	}
	block->next = cur;
	if (prev)
		prev->next = block;
	else
		hdr->free_list = block;

	if (prev && prev->free &&
		(char *)prev + sizeof(t_malloc_block) + prev->size == (char *)block)
	{
		prev->size += sizeof(t_malloc_block) + block->size;
		prev->next = block->next;
		block = prev;
	}

	coalesce_blocks(block);
}

void	*calloc(size_t nmemb, size_t size)
{
	void		*ptr;
	size_t		total;
	uint8_t		*p;

	if (nmemb != 0 && size > SIZE_MAX / nmemb)
		return 0;
	total = nmemb * size;
	ptr = malloc(total);
	if (!ptr)
		return 0;

	p = (uint8_t *)ptr;
	while (total--)
		*p++ = 0;
	return ptr;
}

void	*realloc(void *ptr, size_t size)
{
	t_malloc_block	*block;
	void		*new_ptr;
	uint32_t	old_size;
	uint32_t	to_copy;

	if (!ptr)
		return malloc(size);
	if (size == 0)
	{
		free(ptr);
		return 0;
	}

	block = (t_malloc_block *)((char *)ptr - sizeof(t_malloc_block));
	old_size = block->size;

	if (old_size >= (uint32_t)size)
		return ptr;

	new_ptr = malloc(size);
	if (!new_ptr)
		return 0;

	to_copy = old_size;
	{
		uint8_t	*src = (uint8_t *)ptr;
		uint8_t	*dst = (uint8_t *)new_ptr;
		uint32_t	i;
		for (i = 0; i < to_copy; i++)
			dst[i] = src[i];
	}
	free(ptr);
	return new_ptr;
}

