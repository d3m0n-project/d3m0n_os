
#include "stdlib.h"

char	*strnstr(const char *into, const char *substring, size_t len)
{
	size_t	i;
	size_t	substring_len;

	i = 0;
	substring_len = strlen(substring);
	if (substring_len == 0)
		return ((char *)into);
	if (len <= 0 || substring_len > strlen(into))
		return (0);
	while (into[i] && i + substring_len <= len)
	{
		if (!strncmp(into + i, substring, substring_len))
			return ((char *)&into[i]);
		i++;
	}
	return (0);
}
