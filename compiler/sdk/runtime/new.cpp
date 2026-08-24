#include "memory.h"

extern "C" void	*malloc(size_t size);

void*	operator new(size_t size)
{
	return malloc(size);
}

void*	operator new[](size_t size)
{
	return malloc(size);
}