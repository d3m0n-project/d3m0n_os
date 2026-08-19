/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putptr_count.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:06:03 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/10 11:49:10 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

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
	uart_print("0x");
	printed += 2;
	printed += display_hex((unsigned long)ptr, 1);
	if (f.flags[FLAG_MINUS])
		printed += ft_put_padding_rep(pad_len, pad_char);
	return (printed);
}
