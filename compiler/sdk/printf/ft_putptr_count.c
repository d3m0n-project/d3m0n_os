#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

static int	hex_ptr_len(unsigned long nb)
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

int	ft_putptr_count(void *ptr, t_format f)
{
	int		printed;
	int		len;
	int		pad_len;
	char	pad_char;

	pad_char = ' ';
	printed = 0;
	if (f.flags[FLAG_ZERO] && !(f.flags[FLAG_MINUS] || f.precision > -1))
		pad_char = '0';
	if (!ptr)
		return (ft_putstr_count("(nil)", f));
	len = 2 + hex_ptr_len((unsigned long)ptr);
	pad_len = f.width - len;
	if (pad_len < 0)
		pad_len = 0;
	if (!f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, pad_char);
	write(1, "0x", 2);
	printed += 2;
	printed += display_hex((unsigned long)ptr, 1);
	if (f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, pad_char);
	return (printed);
}

#ifdef __cplusplus
}
#endif