#include "stdlib.h"

int	isxdigit(int c)
{
	if (c >= '0' && c <= '9')
		return 1;
	if (c >= 'A' && c <= 'F')
		return 1;
	return 0;
}