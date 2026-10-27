#include "printf/printf.hpp"
#include "stdlib.h"
#include "stdio.hpp"

static int	put_float(double value, t_format f, t_buf *buffer, int trim_zeros)
{
	char				whole_digits[24];
	char				fraction_digits[19];
	int					precision;
	int					negative;
	int					whole_len;
	int					pad;
	int					i;
	int					start_len;
	int					dot_written;
	unsigned long long	whole;
	unsigned long long	scale;
	unsigned long long	fraction_value;
	double				fraction;

	if (value != value)
		return (buf_putstr(buffer, "nan") ? 3 : -1);
	negative = value < 0;
	if (negative)
		value = -value;
	precision = f.precision < 0 ? 6 : f.precision;
	if (precision > 18)
		precision = 18;
	if (value >= 18446744073709551616.0)
		return (-1);
	whole = (unsigned long long)value;
	fraction = value - (double)whole;
	scale = 1;
	i = 0;
	while (i < precision)
	{
		scale *= 10;
		i++;
	}
	fraction_value = 0;
	if (precision > 0)
	{
		fraction_value = (unsigned long long)
			(fraction * (double)scale + 0.5);
		if (fraction_value >= scale)
		{
			whole++;
			fraction_value = 0;
		}
		i = precision - 1;
		while (i >= 0)
		{
			fraction_digits[i] = '0' + fraction_value % 10;
			fraction_value /= 10;
			i--;
		}
	}
	whole_len = 0;
	do
	{
		whole_digits[whole_len++] = '0' + whole % 10;
		whole /= 10;
	} while (whole);
	pad = f.width - whole_len - (precision > 0 || f.flags[FLAG_HASHTAG])
		- precision
		- (negative || f.flags[FLAG_PLUS] || f.flags[FLAG_SPACE]);
	if (pad < 0)
		pad = 0;
	if (!f.flags[FLAG_MINUS] && !f.flags[FLAG_ZERO])
	{
		i = pad;
		while (i-- > 0)
			if (!buf_putc(buffer, ' '))
				return (-1);
	}
	if (negative)
	{
		if (!buf_putc(buffer, '-'))
			return (-1);
	}
	else if (f.flags[FLAG_PLUS] || f.flags[FLAG_SPACE])
	{
		if (!buf_putc(buffer, f.flags[FLAG_PLUS] ? '+' : ' '))
			return (-1);
	}
	if (!f.flags[FLAG_MINUS] && f.flags[FLAG_ZERO])
	{
		i = pad;
		while (i-- > 0)
			if (!buf_putc(buffer, '0'))
				return (-1);
	}
	while (whole_len > 0)
		if (!buf_putc(buffer, whole_digits[--whole_len]))
			return (-1);
	dot_written = precision > 0 || f.flags[FLAG_HASHTAG];
	if (dot_written)
		if (!buf_putc(buffer, '.'))
			return (-1);
	start_len = buffer->len;
	i = 0;
	while (i < precision)
	{
		if (!buf_putc(buffer, fraction_digits[i]))
			return (-1);
		i++;
	}
	if (trim_zeros && precision > 0)
	{
		while (buffer->len > (size_t)start_len && buffer->data[buffer->len - 1] == '0')
			buffer->len--;
		if (dot_written && buffer->len == (size_t)start_len && buffer->data[buffer->len - 1] == '.')
			buffer->len--;
	}
	if (f.flags[FLAG_MINUS])
	{
		while (pad-- > 0)
			if (!buf_putc(buffer, ' '))
				return (-1);
	}
	return ((int)buffer->len);
}

static int	print_format(struct s_format f, va_list *args, t_buf *buffer)
{
	if (!f.type)
		return -1;

	if (f.type == '%')
		return buf_putc(buffer, '%');

	else if (f.type == 'c')
		return (putchar_count((char)va_arg(*args, int), f, buffer));

	else if (f.type == 's')
		return (putstr_count(va_arg(*args, char *), f, buffer));

	else if (f.type == 'd' || f.type == 'i')
		return (putnbr_count((int)va_arg(*args, int), f, 0, 0, buffer));
	if (f.type == 'u')
	{
		if (f.length == LEN_LL)
			return (ull_putnbr_count(va_arg(*args, unsigned long long), f, 0, 0, buffer));
		if (f.length == LEN_L)
			return (ul_putnbr_count(va_arg(*args, unsigned long), f, 0, 0, buffer));
		return (uint_putnbr_count(va_arg(*args, unsigned int), f, 0, 0, buffer));
	}
	else if (f.type == 'f' || f.type == 'F' || f.type == 'g' || f.type == 'G')
	{
		double value = va_arg(*args, double);
		int is_general = f.type == 'g' || f.type == 'G';
		if (is_general)
		{
			double magnitude = value < 0 ? -value : value;
			int significant = f.precision < 0 ? 6 : f.precision;
			int exponent = 0;
			if (significant < 1)
				significant = 1;
			while (magnitude >= 10.0) { magnitude /= 10.0; exponent++; }
			while (magnitude > 0.0 && magnitude < 1.0) { magnitude *= 10.0; exponent--; }
			/* %g precision counts significant digits, including the first. */
			f.precision = significant - exponent - 1;
			if (f.precision < 0)
				f.precision = 0;
		}
		return put_float(value, f, buffer, is_general);
	}
	else if (f.type == 'p')
		return (putptr_count((void *)va_arg(*args, void *), f, buffer));
	else if (f.type == 'x' || f.type == 'X')
		return (puthex_count((uint32_t)va_arg(*args, uint32_t), f, 0, 0, buffer));
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

int	snprintf(char *str, size_t size, const char *format, ...)
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

	memcpy(str, buffer.data, min(buffer.len, size));
	str[min(buffer.len, size)] = '\0';

	free(buffer.data);
	return ret;
}


int	printf(const char *format, ...)
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
	return (int)buffer.len;
}

int	fprintf(FILE *f, const char *format, ...)
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
	write(f->fd, buffer.data, buffer.len);
	free(buffer.data);
	return (int)buffer.len;
}
