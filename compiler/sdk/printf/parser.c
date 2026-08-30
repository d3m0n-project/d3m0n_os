#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//static int	index_of(char c, char *str)
//{
//	int	i;

//	i = 0;
//	while (str[i])
//	{
//		if (str[i] == c)
//			return (i);
//		i++;
//	}
//	return (-1);
//}

//static void	parse_number(char *str, size_t *i, int *output)
//{
//	if (!isdigit((unsigned char)str[*i]))
//		return ;
//	*output = 0;
//	while (isdigit((unsigned char)str[*i]))
//	{
//		*output = (*output * 10) + (str[*i] - '0');
//		(*i)++;
//	}
//}

t_format	parse_format(char *str, size_t *i)
{
	t_format	f;

	bzero(&f, sizeof(f));
	f.width = WIDTH_UNRESTRICTED;
	f.precision = WIDTH_UNRESTRICTED;

	/* Skip % */
	/* *i already points after % */

	f.type = str[*i];

	if (str[*i])
		(*i)++;

	return (f);
}

#ifdef __cplusplus
}
#endif