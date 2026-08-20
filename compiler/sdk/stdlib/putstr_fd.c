
#include "stdlib.h"

void	putstr_fd(char *str, int fd)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		putchar_fd(str[i], fd);
		i++;
	}
}
