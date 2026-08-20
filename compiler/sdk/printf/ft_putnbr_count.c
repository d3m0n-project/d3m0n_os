#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

static int	number_len(int nb)
{
	int	i;

	i = 0;
	if (nb <= 0)
		i++;
	while (nb != 0)
	{
		i++;
		nb /= 10;
	}
	return (i);
}

static int	handle_plus_flag(t_format f, int is_negative)
{
	int	printed;

	printed = 0;
	if ((f.flags[FLAG_PLUS] || f.flags[FLAG_SPACE]) && !is_negative)
	{
		if (f.flags[FLAG_PLUS])
			putc('+');
		else
			putc(' ');
		printed++;
	}
	return (printed);
}

static int	display_num(int num_len, long nb, int n)
{
	if (num_len > 0)
	{
		if (nb == -2147483648)
			write(1, "2147483648", 10);
		else
		{
			char	*buf = itoa(n);
			if (!buf)
				return 0;
			write(1, buf, strlen(buf));
			free(buf);
			
		}
		return (num_len);
	}
	return (0);
}

int	ft_putnbr_count(int nb, t_format f, int pad_len, int prec_len)
{
	long	n;
	int		num_len;
	int		p;
	char	pad_char;

	n = nb;
	pad_char = ' ';
	if (nb < 0)
		n = -n;
	num_len = (number_len(nb) - (nb < 0)) * !(f.precision == 0 && nb == 0);
	prec_len = f.precision - num_len;
	pad_len = f.width - (num_len + (prec_len > 0) * prec_len + (nb < 0));
	pad_len *= !(pad_len < 0 || f.width == -1);
	if (f.flags[FLAG_ZERO] && !f.flags[FLAG_MINUS] && f.precision < 0)
		pad_char = '0';
	p = 0;
	if ((nb < 0) && pad_char == '0')
		putc('-');
	p += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] == 0), pad_char);
	p += handle_plus_flag(f, (nb < 0));
	if ((nb < 0) && pad_char == ' ')
		putc('-');
	p += ft_put_precision_rep(prec_len) + display_num(num_len, nb, n);
	p += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] != 0), pad_char);
	return (p);
}

#ifdef __cplusplus
}
#endif