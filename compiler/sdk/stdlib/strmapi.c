
#include "stdlib.h"

char	*strmapi(const char *str, char (*f)(unsigned int, char))
{
	char	*output;
	size_t	size;

	size = strlen(str) + 1;
	output = calloc(sizeof(char), size);
	if (!output)
		return (0);
	size -= 1;
	while (size > 0)
	{
		output[size - 1] = (char)(*f)(size - 1, ((char *)str)[size - 1]);
		size--;
	}
	return (output);
}
