#include "stdlib.h"
#include "sys.h"

size_t	strlen(const char *str)
{
	size_t	size;
	write(1, "strlen: '", 10);

	size = 0;
	while (((char *)str)[size] != '\0')
	{
		write(1, str + size, 1);
		size++;
	}
	write(1, "'\n", 2);
	return (size);
}
