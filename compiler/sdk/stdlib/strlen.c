#include "stdlib.h"

size_t	strlen(const char *str)
{
	size_t	size;

	size = 0;
	while (str[size] != '\0')
		size++;
	return (size);
}
