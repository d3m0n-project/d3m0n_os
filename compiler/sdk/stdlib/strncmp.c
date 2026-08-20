
#include "stdlib.h"

int	strncmp(const char *s1, const char *s2, size_t len)
{
	size_t	i;

	i = 0;
	if (len == 0)
		return (0);
	while (s1[i] && s2[i] && s1[i] == s2[i] && i < len - 1)
		i++;
	return (((unsigned char)s1[i]) - (unsigned char)s2[i]);
}
