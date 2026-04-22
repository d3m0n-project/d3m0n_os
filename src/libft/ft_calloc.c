/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:15:10 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/11 14:12:48 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	size_t	total_size;
	char	*buff;

	if (nmemb > 0 && size > SIZE_MAX / nmemb)
		return (0);
	total_size = nmemb * size;
	if (total_size == 0)
		return (malloc(0));
	buff = malloc(sizeof(char) * total_size);
	if (!buff)
		return (0);
	ft_memset(buff, 0, total_size);
	return (buff);
}
