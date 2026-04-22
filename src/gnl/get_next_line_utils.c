#include "get_next_line.h"

void	clear_buffer(char buffer[BUFFER_SIZE + 1])
{
	uint32_t	i;

	i = 0;
	while (buffer && i < BUFFER_SIZE + 1)
	{
		buffer[i] = '\0';
		i++;
	}
}

char	*ft_strcat_realloc(char *s1, char *s2, uint32_t n)
{
	uint32_t	s1_len;
	uint32_t	i;
	char		*new;

	s1_len = ft_strlen(s1);
	new = malloc(s1_len + n + 1);
	if (!new)
		return (0);
	i = 0;
	while (s1 && s1[i])
	{
		new[i] = s1[i];
		i++;
	}
	if (s1)
		free(s1);
	s1 = 0;
	while (n-- > 0 && s2 && *s2)
		new[i++] = *(s2++);
	new[i] = '\0';
	return (new);
}
