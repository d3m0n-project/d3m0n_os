
#include "stdlib.h"

void	striteri(char *str, void (*f)(unsigned int, char*))
{
	size_t	size;

	size = strlen(str);
	while (size > 0)
	{
		(*f)(size - 1, &((char *)str)[size - 1]);
		size--;
	}
}
