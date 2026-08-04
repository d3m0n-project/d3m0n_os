/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 16:36:35 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/04 16:38:45 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_realloc(char *buffer, size_t new_size)
{
	char	*new_buff = malloc(new_size);
	if (!new_buff)
		return 0;
	ft_strlcpy(new_buff, buffer, new_size);
	free(buffer);
	return new_buff;
}