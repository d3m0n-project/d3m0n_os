
#include "stdlib.h"

char	*substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	s_len;
	size_t	sub_len;

	if (!s)
		return (NULL);
	s_len = strlen(s);
	if (start >= s_len)
		return (strdup(""));
	if (len > s_len - start)
		sub_len = s_len - start;
	else
		sub_len = len;
	substr = calloc(sub_len + 1, sizeof(char));
	if (!substr)
		return (NULL);
	strlcpy(substr, s + start, sub_len + 1);
	return (substr);
}
