#include "stdlib.h"

int	strcasecmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while (s1[i] || s2[i])
	{
		if (tolower(s1[i]) != tolower(s2[i]))
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}