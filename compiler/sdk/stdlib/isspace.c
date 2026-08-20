
#include "stdlib.h"

int	isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' ||
		c == '\r' || c == '\v' || c == '\f');
}
