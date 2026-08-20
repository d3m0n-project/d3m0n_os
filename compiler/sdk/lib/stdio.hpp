#ifndef STDIO_H
#define STDIO_H

#include "types.h"
#include "sys.h"
#include "printf/printf.hpp"

extern "C" {
	int	print_format(struct s_format f, va_list args)
	{
		if (!f.type)
			return (-1);
		if (f.type == '%')
		{
			write(1, "%", 1);
			return 1;
		}
		else if (f.type == 'c')
		{
			return (ft_putchar_count((char)va_arg(args, int), f));
		}
		else if (f.type == 's')
		{
			return (ft_putstr_count(va_arg(args, char *), f));
		}
		else if (f.type == 'd' || f.type == 'i')
			return (ft_putnbr_count((int)va_arg(args, int), f, 0, 0));
		if (f.type == 'u')
		{
			if (f.length == LEN_LL)
				return (ft_ull_putnbr_count(va_arg(args, unsigned long long), f, 0, 0));
			if (f.length == LEN_L)
				return (ft_ul_putnbr_count(va_arg(args, unsigned long), f, 0, 0));
			return (ft_uint_putnbr_count(va_arg(args, unsigned int), f, 0, 0));
		}
		else if (f.type == 'p')
			return (ft_putptr_count((void *)va_arg(args, void *), f));
		else if (f.type == 'x' || f.type == 'X')
			return (ft_puthex_count((uint32_t)va_arg(args, uint32_t), f, 0, 0));
		return (-1);
	}

	static inline size_t	printf(const char *format, ...)
	{
		va_list	args;
		size_t	i;
		int		j;
		size_t	len;

		i = 0;
		len = 0;
		va_start(args, format);
		while (((char *)format)[i])
		{
			if (((char *)format)[i] == '%')
			{
				i++;
				j = print_format(parse_format((char *)format, &i), args);
				if (j < 0)
					return (-1);
				len += j - 1;
			}
			else
				write(1, &((char *)format)[i++], 1);
			len++;
		}
		va_end(args);
		return (len);
	}
}

#endif