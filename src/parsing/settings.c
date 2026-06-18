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

typedef struct s_conf_elem
{
	const char		*name;
	void			*config_ptr;
	e_conf_type		type;
}	t_conf_elem;

int		parse_config(t_conf *config)
{
	t_conf_elem	CONFIG_SCHEMA[] = {
		{"wallpaper", &config->wallpaper, TYPE_STRING},
		{"theme", &config->theme, TYPE_STRING},
		{"splash_time", &config->splash_time, TYPE_INT},
		{"time_mode", &config->time_mode, TYPE_INT},
		{0}
	};
	if (!config)
		return 1;
	
	t_conf_elem	*current = 0;
	int			i = 0;
	while ((current = &(CONFIG_SCHEMA[i])))
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
			*(int *)current->config_ptr = ft_atoi(value);
		free(value);
		i++;
	}
	
	return 0;
}

/*
	returns the value of the setting,
	- 0 if an error occured
	- 1 if not found
*/
char	*get_setting(const char *path, const char *key)
{
	char	*line = 0;
	int		fd = 0;
	fd = open(path, O_READ);
	if (fd < 0)
		return 0;
	while ((line = get_next_line(fd)))
	{
		if (line[0] == '#' || line[0] == '\r' || line[0] == '\n' || line[0] == ' ') // skip comments and new lines
		{
			free(line);
			continue;
		}
		int i = 0;
		while (line[i] && line[i] != ':')
			i++;
		if (line[i] == ':')
		{
			if (ft_strlen(key) == (size_t)i && !ft_strncmp(line, key, i))
			{
				i++;
				while (line[i] && (line[i] == ' ' || line[i] == '\t'))
					i++;
				if (!line[i])
				{
					free(line);
					close(fd);
					return 0;
				}

				size_t value_len = 0;
				while (line[i + value_len] && line[i + value_len] != '\n')
					value_len++;
				
				while (value_len > 0 && (line[i + value_len - 1] == ' ' || line[i + value_len - 1] == '\t'))
					value_len--;

				if (value_len == 0)
				{
					//log("Could not find value associated with key: %s\n", LOG_ERROR, key);
					free(line);
					close(fd);
					return (char *)1;
				}
				
				
				char	*output = ft_calloc((value_len + 1), sizeof(char));
				if (!output)
				{
					log("SETTINGS: Could not allocate return setting value\n", LOG_ERROR);
					free(line);
					close(fd);
					return 0;
				}
				ft_strlcpy(output, line + i, value_len);
				free(line);
				close(fd);
				return output;
			}
		} else { // invalid lines
			log("SETTINGS: Invalid line at l.%i: '%s'\n", LOG_ERROR, i, line);
			free(line);
			close(fd);
			return 0;
		}
		free(line);
	}
	close(fd);
	return 0;
}