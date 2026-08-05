/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aviscogl <aviscogl@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 16:36:35 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/06 01:02:31 by aviscogl         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char *ft_realloc(char *buffer, size_t old_size, size_t new_size)
{
    char *new_buff;

    new_buff = malloc(new_size);
    if (!new_buff)
        return 0;

    ft_memcpy(new_buff, buffer, old_size);
    free(buffer);
    return new_buff;
}