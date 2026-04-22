/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:30:28 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/06 13:24:34 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t len)
{
	size_t	i;

	i = 0;
	if (len == 0)
		return (0);
	while (s1[i] && s2[i] && s1[i] == s2[i] && i < len - 1)
		i++;
	return (((unsigned char)s1[i]) - (unsigned char)s2[i]);
}
