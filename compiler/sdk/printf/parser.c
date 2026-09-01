#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

static int	index_of(char c, char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

static void	parse_number(char *str, size_t *i, int *output)
{
	while (isdigit(str[*i]))
	{
		*output = atoi(&str[*i]);
		while (isdigit(str[*i]))
			(*i)++;
	}
}

t_format	parse_format(char *str, size_t *i)
{
	t_format	f;

	bzero(&f, sizeof(f));
	f.width = WIDTH_UNRESTRICTED;
	f.precision = WIDTH_UNRESTRICTED;
	while (str[*i] && strchr("-0# +", str[*i]))
	{
		f.flags[index_of(str[*i], "-0# +") % 5] = str[*i];
		(*i)++;
	}
	parse_number(str, i, &f.width);
	if (str[*i] == '.')
	{
		f.precision = 0;
		(*i)++;
		parse_number(str, i, &f.precision);
	}

	while (str[(*i)] == 'l' && f.length < LEN_LL)
	{
		f.length++;
		(*i)++;
	}

	f.type = str[(*i)++];
	return (f);
}

#ifdef __cplusplus
}
#endif