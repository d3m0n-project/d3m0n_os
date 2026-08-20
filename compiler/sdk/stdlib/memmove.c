
#include "stdlib.h"

void	*memmove(void *dest, const void *src, size_t n)
{
	size_t	i;
	size_t	copy_direction;

	i = 0;
	if (!dest && !src)
		return (0);
	copy_direction = 1;
	if (src < dest)
	{
		i = n - 1;
		copy_direction = -1;
	}
	while (i < n && (int)i >= 0)
	{
		((char *)dest)[i] = ((char *)src)[i];
		i += copy_direction;
	}
	return (dest);
}
