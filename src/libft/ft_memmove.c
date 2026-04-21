/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 09:55:12 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/07 16:35:26 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t	i;
	size_t	copy_direction;

	i = 0;
	if (!dest && !src)
		return (0);
	copy_direction = 1;
	if (src < dest)
	{
		i = n - 1;
		copy_direction = -1;
	}
	while (i < n && (int)i >= 0)
	{
		((char *)dest)[i] = ((char *)src)[i];
		i += copy_direction;
	}
	return (dest);
}
