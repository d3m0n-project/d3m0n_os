#include "memory.h"

extern "C" void	free(void *ptr);

void operator	delete(void* ptr) noexcept
{
	free(ptr);
}
void operator	delete[](void* ptr) noexcept
{
	free(ptr);
}
void operator	delete(void* ptr, size_t) noexcept
{
	free(ptr);
}
void operator	delete[](void* ptr, size_t) noexcept
{
	free(ptr);
}