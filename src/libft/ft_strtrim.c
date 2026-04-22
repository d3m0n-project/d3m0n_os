/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:37:07 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/10 15:34:58 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_in_charset(char c, const char *set)
{
	size_t	i;

	i = 0;
	while (((char *)set)[i] && ((char *)set)[i] != c)
		i++;
	return (((char *)set)[i] == c);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	char	*new_str;
	size_t	i;
	size_t	j;
	size_t	new_str_pos;

	i = 0;
	while (((char *)s1)[i] && is_in_charset(((char *)s1)[i], set))
		i++;
	j = ft_strlen(s1);
	if (i == j)
		return (ft_strdup(""));
	j--;
	while (((char *)s1)[j] && is_in_charset(((char *)s1)[j], set))
		j--;
	new_str = ft_calloc(sizeof(char), j - i + 2);
	new_str_pos = 0;
	if (!new_str)
		return (0);
	while (i <= j)
	{
		new_str[new_str_pos++] = ((char *)s1)[i];
		i++;
	}
	return (new_str);
}
