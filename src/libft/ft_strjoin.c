/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:29:01 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/06 17:23:13 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	size_t	s1_size;
	size_t	s2_size;
	char	*joined_str;

	s1_size = ft_strlen(s1);
	s2_size = ft_strlen(s2);
	joined_str = malloc(sizeof(char) * (s1_size + 1 + s2_size));
	if (!joined_str)
		return (0);
	ft_strlcpy(joined_str, s1, s1_size + 1);
	ft_strlcat(joined_str, s2, (s1_size + 1 + s2_size));
	return (joined_str);
}
