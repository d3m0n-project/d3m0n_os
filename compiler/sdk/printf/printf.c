#include "printf/printf.hpp"
#include "stdlib.h"

static int	print_format(struct s_format f, va_list *args, t_buf *buffer)
{
	if (!f.type)
		return -1;

	if (f.type == '%')
		return buf_putc(buffer, '%');

	else if (f.type == 'c')
		return (ft_putchar_count((char)va_arg(*args, int), f, buffer));


	else if (f.type == 's')
		return (ft_putstr_count(va_arg(*args, char *), f, buffer));

	else if (f.type == 'd' || f.type == 'i')
		return (ft_putnbr_count((int)va_arg(*args, int), f, 0, 0, buffer));
	if (f.type == 'u')
	{
		if (f.length == LEN_LL)
			return (ft_ull_putnbr_count(va_arg(*args, unsigned long long), f, 0, 0, buffer));
		if (f.length == LEN_L)
			return (ft_ul_putnbr_count(va_arg(*args, unsigned long), f, 0, 0, buffer));
		return (ft_uint_putnbr_count(va_arg(*args, unsigned int), f, 0, 0, buffer));
	}
	else if (f.type == 'p')
		return (ft_putptr_count((void *)va_arg(*args, void *), f, buffer));
	else if (f.type == 'x' || f.type == 'X')
		return (ft_puthex_count((uint32_t)va_arg(*args, uint32_t), f, 0, 0, buffer));
	return -1;
}

char	*sprintf(const char *format, ...)
{
	(void)format;
	// TODO: sprintf
	return 0;
}

//size_t printf(const char *format, ...)
//{
//	va_list args;
//	va_start(args, format);
//	size_t len = strlen((char *)format);
//	write(1, format, len);
//	va_end(args);
//	return len;
//}

size_t	printf(const char *format, ...)
{
	va_list	args;
	t_buf	buffer;
	size_t	i;
	int		ret;

	buffer.data = 0;
	buffer.len = 0;
	buffer.cap = 0;
	va_start(args, format);
	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
		{
			i++;
			ret = print_format(parse_format((char *)format, &i), &args, &buffer);
			if (ret < 0)
			{
				va_end(args);
				free(buffer.data);
				return (0);
			}
		}
		else
		{
			if (!buf_putc(&buffer, format[i]))
			{
				va_end(args);
				free(buffer.data);
				return (0);
			}
			i++;
		}
	}

	va_end(args);

	write(1, buffer.data, buffer.len);
	free(buffer.data);
	return (buffer.len);
}