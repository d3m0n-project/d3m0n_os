
#include "stdlib.h"

char	*strncat(char *dst, const char *src, size_t n)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (dst[i])
		i++;

	j = 0;
	while (src[j] && j < n)
	{
		dst[i + j] = src[j];
		j++;
	}

	dst[i + j] = '\0';
	return (dst);
}