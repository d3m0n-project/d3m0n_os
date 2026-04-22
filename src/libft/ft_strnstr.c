/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:27:48 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/10 14:47:04 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *into, const char *substring, size_t len)
{
	size_t	i;
	size_t	substring_len;

	i = 0;
	substring_len = ft_strlen(substring);
	if (substring_len == 0)
		return ((char *)into);
	if (len <= 0 || substring_len > ft_strlen(into))
		return (0);
	while (into[i] && i + substring_len <= len)
	{
		if (!ft_strncmp(into + i, substring, substring_len))
			return ((char *)&into[i]);
		i++;
	}
	return (0);
}
