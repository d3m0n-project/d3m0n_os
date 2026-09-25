#include "stdlib.h"

size_t	strspn(const char *s, const char *accept)
{
	size_t i;
	size_t j;

	i = 0;
	while (s[i] != '\0')
	{
		j = 0;
		while (accept[j] != '\0')
		{
			if (s[i] == accept[j])
				break;
			j++;
		}
		if (accept[j] == '\0')
			return i;
		i++;
	}
	return i;
}