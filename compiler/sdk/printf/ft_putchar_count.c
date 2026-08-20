#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int	ft_putchar_count(char c, t_format f)
{
	int	printed;
	int	pad_len;

	printed = 0;
	pad_len = f.width - 1;
	if (!f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, ' ');
	putc(c);
	printed++;
	if (f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, ' ');
	return (printed);
}

#ifdef __cplusplus
}
#endif