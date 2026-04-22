/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 12:57:37 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/07 13:05:36 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *str, void (*f)(unsigned int, char*))
{
	size_t	size;

	size = ft_strlen(str);
	while (size > 0)
	{
		(*f)(size - 1, &((char *)str)[size - 1]);
		size--;
	}
}
