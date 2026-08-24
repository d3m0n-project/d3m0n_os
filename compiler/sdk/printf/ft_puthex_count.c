#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

static int	hex_len(unsigned int nb)
{
	int	i;

	i = 0;
	if (nb <= 0)
		i++;
	while (nb != 0)
	{
		i++;
		nb /= 16;
	}
	return (i);
}

static int	display_padding_hex(unsigned int i, t_format f, t_buf *buffer)
{
	int	len;

	len = 0;
	if (i == 0)
	{
		if (f.precision != 0)
			buf_putc(buffer, '0');
		else
			len += ft_put_padding_rep(f.precision, ' ', buffer) - 1;
		len++;
	}
	else
		len += display_hex(i, (f.type == 'x'), buffer);
	return (len);
}

int	ft_puthex_count(unsigned int i, t_format f, int pad_len, int prec_len, t_buf *buffer)
{
	int		len;
	int		num_len;
	char	pad_char;

	len = 0;
	num_len = 0;
	pad_char = ' ';
	if (f.flags[FLAG_HASHTAG])
	{
		len += 2;
		buf_putc(buffer, '0');
		buf_putc(buffer, 'X' + (32 * (f.type == 'x')));
	}
	if (!f.flags[FLAG_MINUS] && f.flags[FLAG_ZERO] && f.precision == WIDTH_UNRESTRICTED)
		pad_char = '0';
	num_len = len + hex_len(i);
	if (f.width != WIDTH_UNRESTRICTED)
		pad_len = f.width - max(num_len, f.precision) + (f.precision == 0 && i == 0);
	if (f.precision != WIDTH_UNRESTRICTED)
		prec_len = f.precision - num_len;
	len += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] == 0), pad_char, buffer);
	len += ft_put_precision_rep(prec_len, buffer);
	len += display_padding_hex(i, f, buffer);
	len += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] != 0), pad_char, buffer);
	return (len);
}

#ifdef __cplusplus
}
#endif