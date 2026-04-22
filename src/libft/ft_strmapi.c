/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 15:40:29 by julcleme          #+#    #+#             */
/*   Updated: 2025/11/07 12:56:45 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(const char *str, char (*f)(unsigned int, char))
{
	char	*output;
	size_t	size;

	size = ft_strlen(str) + 1;
	output = ft_calloc(sizeof(char), size);
	if (!output)
		return (0);
	size -= 1;
	while (size > 0)
	{
		output[size - 1] = (char)(*f)(size - 1, ((char *)str)[size - 1]);
		size--;
	}
	return (output);
}
