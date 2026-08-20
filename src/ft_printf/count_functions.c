/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_functions.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 15:17:18 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/10 13:51:35 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_put_padding_rep(int count, char c)
{
	int	i;

	i = 0;
	while (i < count)
	{
		uart_putc(c);
		i++;
	}
	return (i);
}

int	ft_put_precision_rep(int count)
{
	return (ft_put_padding_rep(count, '0'));
}

int	ft_putstr_count(char *str, t_format f)
{
	int	len;
	int	printed;
	int	pad_len;
	int	is_null;

	//uart_print(str);

	is_null = (!str);
	if (is_null)
		str = "(null)";
	len = ft_strlen(str);
	if (f.precision != -1 && len > f.precision && is_null)
		len = 0;
	else if (f.precision > -1 && f.precision < len)
		len = f.precision;
	pad_len = f.width - len;
	if (pad_len < 0)
		pad_len = 0;
	printed = 0;
	if (!f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, ' ');
	if (!is_null || f.precision >= 6 || f.precision == -1)
		uart_print((char *)str);
	printed += len;
	if (f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, ' ');
	return (printed);
}
