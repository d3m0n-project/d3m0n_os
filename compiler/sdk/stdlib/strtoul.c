#include "stdlib.h"

unsigned long	strtoul(const char *nptr, char **endptr, int base)
{
	const char		*p = nptr;
	unsigned long	result = 0;

	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		++p;

	if (base == 0) {
		if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
		{
			base = 16;
			p += 2;
		}
		else if (p[0] == '0')
		{
			base = 8;
			++p;
		}
		else
			base = 10;
	}
	else if (base == 16 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
		p += 2;

	while (*p)
	{
		unsigned int	digit;

		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else if (*p >= 'a' && *p <= 'z')
			digit = *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'Z')
			digit = *p - 'A' + 10;
		else
			break;

		if (digit >= (unsigned int)base)
			break;
		result = result * (unsigned int)base + digit;
		++p;
	}
	if (endptr)
		*endptr = (char *)p;

	return result;
}