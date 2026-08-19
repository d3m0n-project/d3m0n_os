/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 00:15:44 by julcleme          #+#    #+#             */
/*   Updated: 2026/08/10 14:40:09 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	index_of(char c, char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

static void	parse_number(char *str, size_t *i, int *output)
{
	while (ft_isdigit(str[*i]))
	{
		*output = ft_atoi(&str[*i]);
		while (ft_isdigit(str[*i]))
			(*i)++;
	}
}

t_format	parse_format(char *str, size_t *i)
{
	t_format	f;

	ft_memset(&f.flags, 0, 6);
	f.width = WIDTH_UNRESTRICTED;
	f.precision = WIDTH_UNRESTRICTED;
	while (str[*i] && ft_strchr("-0# +", str[*i]))
	{
		f.flags[index_of(str[*i], "-0# +") % 5] = str[*i];
		(*i)++;
	}
	parse_number(str, i, &f.width);
	if (str[*i] == '.')
	{
		f.precision = 0;
		(*i)++;
		parse_number(str, i, &f.precision);
	}

	while (str[(*i)] == 'l' && f.length < LEN_LL)
	{
		f.length++;
		(*i)++;
	}

	f.type = str[(*i)++];
	return (f);
}
