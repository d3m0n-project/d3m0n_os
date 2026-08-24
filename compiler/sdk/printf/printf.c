#include "printf/printf.hpp"
#include "stdlib.h"

static size_t	print_format(struct s_format f, va_list args, t_buf *buffer)
{
	if (!f.type)
		return 0;

	if (f.type == '%')
		return buf_putc(buffer, '%');

	else if (f.type == 'c')
		return (ft_putchar_count((char)va_arg(args, int), f, buffer));


	else if (f.type == 's')
		return (ft_putstr_count(va_arg(args, char *), f, buffer));

	else if (f.type == 'd' || f.type == 'i')
		return (ft_putnbr_count((int)va_arg(args, int), f, 0, 0, buffer));
	if (f.type == 'u')
	{
		if (f.length == LEN_LL)
			return (ft_ull_putnbr_count(va_arg(args, unsigned long long), f, 0, 0, buffer));
		if (f.length == LEN_L)
			return (ft_ul_putnbr_count(va_arg(args, unsigned long), f, 0, 0, buffer));
		return (ft_uint_putnbr_count(va_arg(args, unsigned int), f, 0, 0, buffer));
	}
	else if (f.type == 'p')
		return (ft_putptr_count((void *)va_arg(args, void *), f, buffer));
	else if (f.type == 'x' || f.type == 'X')
		return (ft_puthex_count((uint32_t)va_arg(args, uint32_t), f, 0, 0, buffer));
	return 0;
}

char	*sprintf(const char *format, ...)
{
	va_list		args;
	char		*output = malloc(1);
	if (!output)
		return 0;
	size_t		i;
	size_t		j = 0;

	i = 0;

	t_buf	buffer = {.data=output, .len=1, .cap=0};
	va_start(args, format);
	while (((char *)format)[i])
	{
		if (((char *)format)[i] == '%')
		{
			i++;
			j = print_format(parse_format((char *)format, &i), args, &buffer);
			if (j == 0)
			{
				free(output);
				return 0;
			}
		}
		else
			buf_putc(&buffer, ((char *)format)[i++]);
	}
	va_end(args);
	return output;
}

size_t printf(const char *format, ...)
{
	va_list args;
    va_start(args, format);
    size_t len = strlen((char *)format);
	write(1, format, len);
	va_end(args);
    return len;
}

//size_t	printf(const char *format, ...)
//{
//	va_list		args;
//	char		*output = malloc(1);
//	if (!output)
//		return 0;
//	size_t		i;
//	size_t		j = 0;
//	size_t		len;

//	i = 0;
//	len = 0;

//	t_buf	buffer = {.data=output, .len=1, .cap=0};
//	va_start(args, format);
//	while (((char *)format)[i])
//	{
//		if (((char *)format)[i] == '%')
//		{
//			i++;
//			j = print_format(parse_format((char *)format, &i), args, &buffer);
//			if (j == 0)
//			{
//				free(output);
//				return 0;
//			}
//			len += j - 1;
//		}
//		else
//			buf_putc(&buffer, ((char *)format)[i++]);
//		len++;
//	}
//	va_end(args);
//	write(1, output, len);
//	return len;
//}