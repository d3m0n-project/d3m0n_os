
#include "stdlib.h"

char	*strrchr(const char *str, int c)
{
	size_t	i;
	char	*last;

	i = 0;
	last = 0;
	while (str[i])
	{
		if (str[i] == (char)c)
			last = (char *)(&str[i]);
		i++;
	}
	if (str[i] == (char)c)
		return ((char *)&str[i]);
	return (last);
}
