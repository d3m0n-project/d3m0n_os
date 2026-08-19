/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_uint_putnbr_count.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:22:19 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/10 14:47:42 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_uint_putnbr_count(unsigned int nb, t_format f, int pad_len, int prec_len)
{
	return ft_ull_putnbr_count((unsigned long long)nb, f, pad_len, prec_len);
}

int	ft_ul_putnbr_count(unsigned long nb, t_format f, int pad_len, int prec_len)
{
	return ft_ull_putnbr_count((unsigned long long)nb, f, pad_len, prec_len);
}


