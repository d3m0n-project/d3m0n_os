/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types_display.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 00:03:18 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/10 14:44:58 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	display_hex(size_t nb, int in_lowercase)
{
	char	output;
	int		len;

	len = 0;
	if (nb >= 16)
		len += display_hex(nb / 16, in_lowercase);
	output = nb % 16;
	if (output >= 10)
		output = (output - 10) + ('A' + 32 * (in_lowercase != 0));
	else
		output = output + '0';
	uart_putc(output);
	return (len + 1);
}

int	display_uint(unsigned int nb)
{
	int	len;

	len = 0;
	if (nb == 0)
		return (0);
	if (nb > 0)
		len += 1 + display_uint(nb / 10);
	uart_putc('0' + nb % 10);
	return (len);
}

int	display_ull(unsigned long long nb)
{
	int	len;

	len = 0;
	if (nb == 0)
		return (0);
	if (nb > 0)
		len += 1 + display_ull(nb / 10);
	uart_putc('0' + nb % 10);
	return (len);
}
