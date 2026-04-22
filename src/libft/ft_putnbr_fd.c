/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 13:11:01 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/07 13:44:40 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	get_exp_of_ten_len(int nb)
{
	int	i;

	i = 0;
	if (nb < 0)
		i++;
	if (nb == 0)
		return (1);
	while (nb != 0)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

static int	handle_special_cases(char *output, int nb)
{
	if (nb == -2147483648)
	{
		ft_strlcpy(output, "-2147483648", 13);
		return (1);
	}
	else if (nb == 0)
	{
		ft_strlcpy(output, "0", 2);
		return (1);
	}
	return (0);
}

void	ft_itoa_static(int nb, char *buffer, size_t buffer_size)
{
	size_t	i;
	size_t	exp;

	exp = get_exp_of_ten_len(nb);
	if (!buffer)
		return ;
	ft_memset(buffer, 0, buffer_size);
	if (handle_special_cases(buffer, nb))
		return ;
	i = 0;
	if (nb < 0)
	{
		buffer[0] = '-';
		nb = -nb;
	}
	while (nb > 0)
	{
		buffer[exp - ++i] = (nb % 10) + '0';
		nb /= 10;
	}
}

void	ft_putnbr_fd(int n, int fd)
{
	char	buffer[13];

	ft_itoa_static(n, buffer, 13);
	ft_putstr_fd(buffer, fd);
}
