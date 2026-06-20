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
	void			*default_value;
}	t_conf_elem;

int		parse_config(t_conf *config)
{
	t_conf_elem	CONFIG_SCHEMA[] = {
		{"wallpaper", &config->wallpaper, TYPE_STRING, {0}, "default.bmp"},
		{"theme", &config->theme, TYPE_STRING, {0}, "default_dark"},
		{"splash_time", &config->splash_time, TYPE_INT, {.min=0, .max=5000}, &(int){3000}},
		{"time_mode", &config->time_mode, TYPE_INT, {.min=0, .max=1}, &(int){0}},
		{0}
	};
	if (!config)
		return 1;
	
	t_conf_elem	*current = 0;
	int			i = 0;
	while (*(char *)(current = &(CONFIG_SCHEMA[i])))
	{
		// parse defaults
		int not_found = 0;
		char	*value = get_setting("/config", current->name, &not_found);
		if (!value && !not_found) // error
			return 1;
		else if (not_found) // not found, use default
		{
			if (current->type == TYPE_STRING)
				ft_strlcpy((char *)current->config_ptr, current->default_value, ft_strlen(current->default_value));
			else if (current->type == TYPE_INT)
				*(int *)current->config_ptr = *(int *)current->default_value;
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

char	*get_setting(const char *path, const char *key, int	*not_found)
{
	char	*line;
	int		fd;

	fd = open(path, O_READ);
	if (fd < 0)
		return 0;

	*not_found = 0;
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
				*not_found = 1;
				return 0;
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