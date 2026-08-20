
#include "stdlib.h"
#include "sys.h"

void	putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
