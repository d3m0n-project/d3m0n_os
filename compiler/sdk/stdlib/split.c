
#include "stdlib.h"

static int	len_next_part(const char *str, char c)
{
	size_t	count;

	count = 0;
	while (str[count] && str[count] != c)
		count++;
	return (count);
}

static int	count_words(const char *str, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (str[i])
	{
		while (str[i] && str[i] == c)
			i++;
		if (str[i])
			count++;
		while (str[i] && str[i] != c)
			i++;
	}
	return (count);
}

static char	**clean_strings(char **output, size_t k)
{
	while (k > 0)
		free(output[--k]);
	free(output);
	return (0);
}

void	cleanup_splitted(char **splitted)
{
	if (!splitted)
		return;
	int i = 0;
	while (splitted[i])
	{
		free(splitted[i]);
		i++;
	}
	free(splitted);
}

char	**split(const char *str, char c)
{
	char	**output;
	size_t	k;
	size_t	len;

	k = 0;
	if (!str)
		return (0);
	output = calloc(count_words(str, c) + 1, sizeof(char *));
	if (!output)
		return (0);
	while (*str)
	{
		while (*str && *str == c)
			str++;
		if (*str)
		{
			len = len_next_part(str, c);
			output[k] = calloc(len + 1, sizeof(char));
			if (!output[k])
				return (clean_strings(output, k));
			memcpy(output[k++], str, len);
			str += len;
		}
	}
	return (output);
}
