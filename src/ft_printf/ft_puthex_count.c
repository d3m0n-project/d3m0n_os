/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthex_count.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:20:40 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/10 13:51:12 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	hex_len(unsigned int nb)
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

static int	display_padding_hex(unsigned int i, t_format f)
{
	int	len;

	len = 0;
	if (i == 0)
	{
		if (f.precision != 0)
			uart_putc('0');
		else
			len += ft_put_padding_rep(f.precision, ' ') - 1;
		len++;
	}
	else
		len += display_hex(i, (f.type == 'x'));
	return (len);
}

int	ft_puthex_count(unsigned int i, t_format f, int pad_len, int prec_len)
{
	int		len;
	int		num_len;
	char	pad_char;

	len = 0;
	num_len = 0;
	pad_char = ' ';
	if (f.flags[FLAG_HASHTAG])
	{
		len += 2;
		uart_putc('0');
		uart_putc('X' + (32 * (f.type == 'x')));
	}
	if (!f.flags[FLAG_MINUS] && f.flags[FLAG_ZERO] && f.precision == WIDTH_UNRESTRICTED)
		pad_char = '0';
	num_len = len + hex_len(i);
	if (f.width != WIDTH_UNRESTRICTED)
		pad_len = f.width - max(num_len, f.precision) + (f.precision == 0 && i == 0);
	if (f.precision != WIDTH_UNRESTRICTED)
		prec_len = f.precision - num_len;
	len += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] == 0), pad_char);
	len += ft_put_precision_rep(prec_len);
	len += display_padding_hex(i, f);
	len += ft_put_padding_rep(pad_len * (f.flags[FLAG_MINUS] != 0), pad_char);
	return (len);
}
