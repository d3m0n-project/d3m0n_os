/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:14:06 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/06 17:23:13 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	res;
	size_t	size_src;
	size_t	size_dst;

	i = 0;
	size_src = ft_strlen(src);
	size_dst = ft_strlen(dst);
	if (size == 0)
		return (size_src);
	if (size > size_dst)
		res = size_src + size_dst;
	else
		res = size_src + size;
	while (src[i] && (size_dst + i) < size - 1)
	{
		dst[size_dst + i] = src[i];
		i++;
	}
	dst[size_dst + i] = '\0';
	return (res);
}
