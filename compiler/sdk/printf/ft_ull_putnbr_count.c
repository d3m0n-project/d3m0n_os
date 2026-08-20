#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

static int	number_len_ull(unsigned long long nb)
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

int	ft_ull_putnbr_count(unsigned long long nb, t_format f, int pad_len, int prec_len)
{
	int		num_len;
	int		printed;
	char	pad_char;

	pad_char = ' ';
	printed = 0;
	num_len = number_len_ull(nb);
	if (f.precision > num_len)
		prec_len = f.precision - num_len;
	if (f.width != WIDTH_UNRESTRICTED)
		pad_len = f.width - (max(num_len, f.precision) * !(f.precision == 0 && nb == 0));
	if (f.flags[FLAG_ZERO] && !f.flags[FLAG_MINUS] && f.precision < 0)
		pad_char = '0';
	printed += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] == 0), pad_char);
	printed += ft_put_precision_rep(prec_len);
	if (!(f.precision == 0 && nb == 0))
	{
		if (nb == 0)
			putc('0');
		printed += display_ull(nb);
	}
	printed += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] != 0), ' ');
	return (printed + ((nb == 0) * (f.precision != 0)));
}

#ifdef __cplusplus
}
#endif