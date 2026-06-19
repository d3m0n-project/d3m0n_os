#include "parsing.h"
#include "memory.h"
#include "get_next_line.h"
#include "libft.h"
#include "settings.h"

typedef enum
{
	TYPE_STRING,
	TYPE_INT
}	e_conf_type;

typedef struct s_range
{
	int	min;
	int	max;
}	t_range; // range (included)

typedef struct s_conf_elem
{
	const char		*name;
	void			*config_ptr;
	e_conf_type		type;
	t_range			range;
}	t_conf_elem;

int		parse_config(t_conf *config)
{
	t_conf_elem	CONFIG_SCHEMA[] = {
		{"wallpaper", &config->wallpaper, TYPE_STRING, {0}},
		{"theme", &config->theme, TYPE_STRING, {0}},
		{"splash_time", &config->splash_time, TYPE_INT, {.min=0, .max=5000}},
		{"time_mode", &config->time_mode, TYPE_INT, {.min=0, .max=1}},
		{0}
	};
	if (!config)
		return 1;
	
	t_conf_elem	*current = 0;
	int			i = 0;
	while (*(char *)(current = &(CONFIG_SCHEMA[i])))
	{
		char	*value = get_setting("/config", current->name);
		if (!value) // error
			return 1;
		if ((size_t)value == 1) // not found, skip
		{
			i++;
			continue;
		}
		if (current->type == TYPE_STRING)
		{
			size_t	len = ft_strlen(value);
			if (len > STRING_SIZE - 1)
			{
				log("CONFIG: Invalid config entry, max string length: %i\n", LOG_ERROR, STRING_SIZE);
				free(value);
				return 1;
			}
			ft_strlcpy((char *)current->config_ptr, value, len);
		}
		else if (current->type == TYPE_INT)
		{
			int v = ft_atoi(value);
			if (current->range.min <= v && v <= current->range.max)
				*(int *)current->config_ptr = v;
			else
			{
				log("CONFIG: Integer not in allowed range [%i, %i], got %i\n", LOG_ERROR, current->range.min, current->range.max, v);
				free(value);
				return 1;
			}
		}
		free(value);
		i++;
	}
	
	return 0;
}

char	*get_setting(const char *path, const char *key)
{
	char	*line;
	int		fd;

	fd = open(path, O_READ);
	if (fd < 0)
		return 0;

	while ((line = get_next_line(fd)))
	{
		size_t	i;
		size_t	value_len;
		char	*output;

		if (line[0] == '#' || line[0] == '\r' || line[0] == '\n' || line[0] == ' ')
		{
			free(line);
			continue;
		}

		i = 0;
		while (line[i] && line[i] != ':')
			i++;

		if (line[i] != ':')
		{
			free(line);
			close(fd);
			return 0;
		}

		if (ft_strlen(key) == i && !ft_strncmp(line, key, i))
		{
			i++; // skip

			while (line[i] == ' ' || line[i] == '\t')
				i++;

			if (!line[i])
			{
				free(line);
				close(fd);
				return 0;
			}

			value_len = 0;
			while (line[i + value_len] && line[i + value_len] != '\n' && line[i + value_len] != '\r')
				value_len++;

			while (value_len > 0 && (line[i + value_len - 1] == ' ' || line[i + value_len - 1] == '\t'))
				value_len--;

			if (value_len == 0)
			{
				free(line);
				close(fd);
				return (char *)1;
			}

			output = ft_calloc(value_len + 1, sizeof(char));
			if (!output)
			{
				free(line);
				close(fd);
				return 0;
			}

			ft_memcpy(output, line + i, value_len);
			output[value_len] = '\0';

			free(line);
			close(fd);
			return output;
		}
		free(line);
	}
	close(fd);
	return 0;
}