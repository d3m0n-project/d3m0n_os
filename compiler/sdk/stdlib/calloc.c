
#include "stdlib.h"

void	*calloc(size_t nmemb, size_t size)
{
	size_t	total_size;
	char	*buff;

	if (nmemb > 0 && size > SIZE_MAX / nmemb)
		return (0);
	total_size = nmemb * size;
	if (total_size == 0)
		return (malloc(0));
	buff = malloc(sizeof(char) * total_size);
	if (!buff)
		return (0);
	memset(buff, 0, total_size);
	return (buff);
}
