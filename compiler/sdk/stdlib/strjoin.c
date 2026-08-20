
#include "stdlib.h"

char	*strjoin(const char *s1, const char *s2)
{
	size_t	s1_size;
	size_t	s2_size;
	char	*joined_str;

	s1_size = strlen(s1);
	s2_size = strlen(s2);
	joined_str = malloc(sizeof(char) * (s1_size + 1 + s2_size));
	if (!joined_str)
		return (0);
	strlcpy(joined_str, s1, s1_size + 1);
	strlcat(joined_str, s2, (s1_size + 1 + s2_size));
	return (joined_str);
}
