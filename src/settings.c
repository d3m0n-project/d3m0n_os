#include "parsing.h"
#include "memory.h"
#include "get_next_line.h"
#include "libft.h"
#include "settings.h"

#define	SCHEMA_STR(member, key, def)			{key, config->member, TYPE_STRING, {0}, def},
#define	SCHEMA_INT(member, key, def, min_n, max_n)	{key, &config->member, TYPE_INT, {.min=min_n,.max=max_n}, &(int){def}},
#define	SCHEMA(kind, ...)						SCHEMA_##kind(__VA_ARGS__)

t_offset	conf_offsets[] = {
    CONF_FIELDS(OFFSET)
};

int		parse_config(t_conf *config)
{
	t_conf_elem CONFIG_SCHEMA[] = {
		CONF_FIELDS(SCHEMA)
		{0}
	};

	if (!config)
		return 1;
	
	t_conf_elem	*current = 0;
	int			i = 0;
	while ((current = &CONFIG_SCHEMA[i])->name)
	{
		// parse defaults
		int not_found = 0;
		char *value = get_setting("/config", current->name, &not_found);
		if (!value && !not_found) // error
			return 1;
		else if (not_found) // not found, use default
		{
			if (current->type == TYPE_STRING)
				ft_strlcpy((char *)current->config_ptr, current->default_value, ft_strlen(current->default_value) + 1);
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
			ft_strlcpy((char *)current->config_ptr, value, len + 1);
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

	*not_found = 0;
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

int	set_setting(const char *path, const char *key, const char *value)
{
	char	*line;
	int		fd;
	int		found = 0;
	char	**lines = NULL;
	int		line_count = 0;
	int		i;

	if (!path || !key || !value)
		return 1;

	fd = open(path, O_READ);
	if (fd < 0)
		return 1;

	while ((line = get_next_line(fd)))
	{
		line_count++;
		free(line);
	}
	close(fd);

	lines = (char **)ft_calloc(line_count + 2, sizeof(char *));
	if (!lines)
		return 1;

	fd = open(path, O_READ);
	if (fd < 0)
	{
		free(lines);
		return 1;
	}

	i = 0;
	while ((line = get_next_line(fd)) && i < line_count)
	{
		size_t key_len = 0;
		while (line[key_len] && line[key_len] != ':')
			key_len++;

		if (line[key_len] == ':' && ft_strlen(key) == key_len && !ft_strncmp(line, key, key_len))
		{
			size_t new_len = ft_strlen(key) + 2 + ft_strlen(value);
			char *new_line = (char *)ft_calloc(new_len + 1, sizeof(char));
			if (!new_line)
			{
				free(line);
				goto cleanup;
			}
			ft_strlcpy(new_line, key, new_len + 1);
			ft_strlcat(new_line, ": ", new_len + 1);
			ft_strlcat(new_line, value, new_len + 1);
			lines[i] = new_line;
			found = 1;
			free(line);
		}
		else
			lines[i] = line;
		i++;
	}
	close(fd);

	if (!found)
		goto cleanup;

	// Write all lines back to file
	fd = open(path, O_WRITE | O_TRUNC);
	if (fd < 0)
		goto cleanup;

	i = 0;
	while (lines[i])
	{
		size_t len = ft_strlen(lines[i]);
		if (write(fd, lines[i], len) != (int)len)
		{
			close(fd);
			goto cleanup;
		}
		// write newline if not already present
		if (len == 0 || (lines[i][len - 1] != '\n'))
		{
			if (write(fd, "\n", 1) != 1)
			{
				close(fd);
				goto cleanup;
			}
		}
		i++;
	}
	close(fd);

	// cleanup
	i = 0;
	while (lines[i])
	{
		free(lines[i]);
		i++;
	}
	free(lines);
	return 0;

cleanup:
	i = 0;
	while (lines[i])
	{
		free(lines[i]);
		i++;
	}
	free(lines);
	return 1;
}