
#include "stdlib.h"

void	putendl_fd(char *str, int fd)
{
	putstr_fd(str, fd);
	putchar_fd('\n', fd);
}
