
#include "stdlib.h"

static int	is_in_charset(char c, const char *set)
{
	size_t	i;

	i = 0;
	while (((char *)set)[i] && ((char *)set)[i] != c)
		i++;
	return (((char *)set)[i] == c);
}

char	*strtrim(const char *s1, const char *set)
{
	char	*new_str;
	size_t	i;
	size_t	j;
	size_t	new_str_pos;

	i = 0;
	while (((char *)s1)[i] && is_in_charset(((char *)s1)[i], set))
		i++;
	j = strlen(s1);
	if (i == j)
		return (strdup(""));
	j--;
	while (((char *)s1)[j] && is_in_charset(((char *)s1)[j], set))
		j--;
	new_str = calloc(sizeof(char), j - i + 2);
	new_str_pos = 0;
	if (!new_str)
		return (0);
	while (i <= j)
	{
		new_str[new_str_pos++] = ((char *)s1)[i];
		i++;
	}
	return (new_str);
}
