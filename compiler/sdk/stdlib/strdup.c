
#include "stdlib.h"

char	*strdup(const char *src)
{
	int		size;
	int		i;
	char	*cpy;

	size = 0;
	while (src[size])
		size++;
	cpy = malloc(sizeof(char) * (size + 1));
	if (!cpy)
		return (0);
	i = 0;
	while (src[i])
	{
		cpy[i] = src[i];
		i++;
	}
	cpy[i] = '\0';
	return (cpy);
}
