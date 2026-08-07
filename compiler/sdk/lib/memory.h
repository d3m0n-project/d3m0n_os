#ifndef MEMORY_H
#define MEMORY_H

#include "../../../includes/types.h"

#define USER_HEAP_RESERVED		0x100000

void	*malloc(size_t size);
void	free(void *ptr);
void	*calloc(size_t nmemb, size_t size);
void	*realloc(void *ptr, size_t size);

#endif

