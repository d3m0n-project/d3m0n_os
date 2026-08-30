
#include "stdlib.h"

static int	get_exp_of_ten_len(int nb)
{
	int	i;

	i = 0;
	if (nb < 0)
		i++;
	if (nb == 0)
		return (1);
	while (nb != 0)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

static int	handle_special_cases(char *output, int nb)
{
	if (nb == -2147483648)
	{
		strlcpy(output, "-2147483648", 12);
		return (1);
	}
	else if (nb == 0)
	{
		strlcpy(output, "0", 2);
		return (1);
	}
	return (0);
}

char	*itoa(int nb)
{
	char	*output;
	size_t	i;
	size_t	exp;

	exp = get_exp_of_ten_len(nb);
	output = calloc(sizeof(char), (exp + 1));
	if (!output)
		return (0);
	if (handle_special_cases(output, nb))
		return (output);
	i = 0;
	if (nb < 0)
	{
		output[0] = '-';
		nb = -nb;
	}
	while (nb > 0)
	{
		output[exp - ++i] = (nb % 10) + '0';
		nb /= 10;
	}
	return (output);
}
