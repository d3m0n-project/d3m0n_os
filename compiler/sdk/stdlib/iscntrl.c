#include "stdlib.h"

int	iscntrl(int c)
{
	return ((unsigned int)c <= 0x1f) || ((unsigned int)c == 0x7f);
}