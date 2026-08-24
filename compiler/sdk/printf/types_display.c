#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int	display_hex(size_t nb, int in_lowercase, t_buf *buffer)
{
	char	output;
	int		len;

	len = 0;
	if (nb >= 16)
		len += display_hex(nb / 16, in_lowercase, buffer);
	output = nb % 16;
	if (output >= 10)
		output = (output - 10) + ('A' + 32 * (in_lowercase != 0));
	else
		output = output + '0';
	buf_putc(buffer, output);
	return (len + 1);
}

int	display_uint(unsigned int nb, t_buf *buffer)
{
	int	len;

	len = 0;
	if (nb == 0)
		return (0);
	if (nb > 0)
		len += 1 + display_uint(nb / 10, buffer);
	buf_putc(buffer, '0' + nb % 10);
	return (len);
}

int	display_ull(unsigned long long nb, t_buf *buffer)
{
	int	len;

	len = 0;
	if (nb == 0)
		return (0);
	if (nb > 0)
		len += 1 + display_ull(nb / 10, buffer);
	buf_putc(buffer, '0' + nb % 10);
	return (len);
}

#ifdef __cplusplus
}
#endif