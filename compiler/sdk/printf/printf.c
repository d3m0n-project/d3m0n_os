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

static int	vprintf_internal(const char *format, t_buf *buffer, va_list *args)
{
	size_t		i;
	int			ret;

	if (!format || !buffer)
		return -1;

	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
		{
			i++;
			ret = print_format(parse_format((char *)format, &i), args, buffer);
			if (ret < 0)
				return -1;
		}
		else
		{
			if (!buf_putc(buffer, format[i]))
				return -1;
			i++;
		}
	}
	return (int)buffer->len;
}

int	sprintf(char *str, const char *format, ...)
{
	va_list	args;
	t_buf	buffer;
	int		ret;

	if (!str || !format)
		return -1;

	buffer.data = 0;
	buffer.len = 0;
	buffer.cap = 0;

	va_start(args, format);
	ret = vprintf_internal(format, &buffer, &args);
	va_end(args);

	if (ret < 0)
	{
		free(buffer.data);
		return -1;
	}

	memcpy(str, buffer.data, buffer.len);
	str[buffer.len] = '\0';

	free(buffer.data);
	return ret;
}


size_t	printf(const char *format, ...)
{
	va_list	args;
	t_buf	buffer;

	buffer.data = 0;
	buffer.len = 0;
	buffer.cap = 0;
	va_start(args, format);
	if (!vprintf_internal(format, &buffer, &args))
	{
		if (buffer.data)
			free(buffer.data);
		va_end(args);
		return 0;
	}
	va_end(args);
	write(1, buffer.data, buffer.len);
	free(buffer.data);
	return (buffer.len);
}
