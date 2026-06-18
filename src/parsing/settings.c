#include "parsing.h"
#include "memory.h"
#include "get_next_line.h"
#include "libft.h"
#include "settings.h"

typedef struct s_conf_elem
{
	const char	*name;
	void		*config_ptr;
}	t_conf_elem;

const static t_conf_elem	*CONFIG_SCHEMA = {{}, 0};


int		parse_config(t_conf *config)
{
	if (!config)
		return 1;
	
	
	config->wallpaper
}

char	*get_setting(const char *path, const char *key)
{
	char	*line = 0;
	int		fd = 0;
	fd = open(path, O_READ);
	if (fd < 0)
		return 0;
	while ((line = get_next_line(fd)))
	{
		if (line[0] == '#') // skip comments
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
					log("Could not find value associated with key: %s\n", LOG_ERROR, key);
					free(line);
					close(fd);
					return 0;
				}
				
				
				char	*output = ft_calloc((value_len + 1), sizeof(char));
				if (!output)
				{
					log("SETTINGS: Could not allocate return setting value\n", LOG_ERROR);
					free(line);
					close(fd);
					return 0;
				}
				if (ft_strlcpy(output, line + i, value_len) < value_len + 1) // validate copied length
				{
					log("SETTINGS: Could not copy return setting value\n", LOG_ERROR);
					free(output);
					free(line);
					close(fd);
					return 0;
				}
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