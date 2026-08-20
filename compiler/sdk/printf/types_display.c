#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int	display_hex(size_t nb, int in_lowercase)
{
	char	output;
	int		len;

	len = 0;
	if (nb >= 16)
		len += display_hex(nb / 16, in_lowercase);
	output = nb % 16;
	if (output >= 10)
		output = (output - 10) + ('A' + 32 * (in_lowercase != 0));
	else
		output = output + '0';
	putc(output);
	return (len + 1);
}

int	display_uint(unsigned int nb)
{
	int	len;

	len = 0;
	if (nb == 0)
		return (0);
	if (nb > 0)
		len += 1 + display_uint(nb / 10);
	putc('0' + nb % 10);
	return (len);
}

int	display_ull(unsigned long long nb)
{
	int	len;

	len = 0;
	if (nb == 0)
		return (0);
	if (nb > 0)
		len += 1 + display_ull(nb / 10);
	putc('0' + nb % 10);
	return (len);
}

#ifdef __cplusplus
}
#endif