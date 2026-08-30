#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int	ft_putchar_count(char c, t_format f, t_buf *buffer)
{
	int	printed;
	int	pad_len;

	printed = 0;
	pad_len = f.width - 1;
	if (!f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, ' ', buffer);
	buf_putc(buffer, c);
	printed++;
	if (f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, ' ', buffer);
	return (printed);
}

#ifdef __cplusplus
}
#endif