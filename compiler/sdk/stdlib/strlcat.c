
#include "stdlib.h"

size_t	strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	dst_len;
	size_t	src_len;

	dst_len = 0;
	while (dst_len < size && dst[dst_len])
		dst_len++;

	src_len = strlen(src);

	if (dst_len == size)
		return (size + src_len);

	i = 0;
	while (src[i] && dst_len + i + 1 < size)
	{
		dst[dst_len + i] = src[i];
		i++;
	}

	dst[dst_len + i] = '\0';

	return (dst_len + src_len);
}
