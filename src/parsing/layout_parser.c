#include "parsing.h"
#include "get_next_line.h"

int	strncmp(char *s1, char *s2, uint32_t n)
{
	uint32_t	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i] && i < n)
		i++;
	return (s1[i] - s2[i]);
}

typedef	struct s_param_entry
{
	int		len;
	char	*name;
}	t_param_entry;

void	parse_window(int fd, t_window *win)
{
	char	*line;
	int	entries_count = 5;
	t_param_entry	entries[5] = {
		{.len=5, .name="width"},
		{.len=5, .name="title"},
		{.len=6, .name="height"},
		{.len=6, .name="topbar"},
		{.len=8, .name="bg_color"}
	};
	while ((line = get_next_line(fd)))
	{
		if (line[0] <= 13 && line[0] >= 9)
		{
			free(line);
			break;
		}
		int equal_pos = 0;
		int	start_pos = 0;
		while (line[start_pos] == ' ' || (line[start_pos] <= 13 && line[start_pos] >= 9))
			start_pos++;
		while (line[start_pos + equal_pos] && line[start_pos + equal_pos] != '=')
			equal_pos++;
		if (line[start_pos + equal_pos] != '=')
		{
			log("Invalid window config: '%s'\n", LOG_ERROR, line);
			free(line);
			return;
		}
		for (int i=0; i<entries_count && entries[i].len < equal_pos; i++)
		{
			log("'%s'\n", LOG_ERROR, line + start_pos);
			if (entries[i].len != equal_pos - 1) continue;
			if (!strncmp(line + start_pos, entries[i].name, equal_pos - 1))
				log("found: '%s'\n", LOG_SUCCESS, entries[i].name);
		}
		free(line);
	}
	(void)win;
}

int	parse_layout(const char *path, t_window *win)
{
	int			fd = open(path, O_READ);
	int			line_idx = 0;
	char		*line;
	t_control	*current_control = 0;
	t_control	*last_control = win->controls;
	if (fd < 0)
	{
		log("Could not open file: %s\n", LOG_ERROR, path);
		return 1;
	}
	while ((line = get_next_line(fd)))
	{
		line_idx++;
		int i = 0;
		while (line[i] == ' ' || (line[i] <= 13 && line[i] >= 9))
			i++;
		// skip comments
		if (line[i] == '#')
		{
			free(line);
			continue;
		}
		// empty line
		if (line[i] == '\n' || line[i] == '\0' || line[i] == '\r')
		{
			if (current_control)
			{
				current_control->p_next = 0;
				last_control->p_next = current_control;
				last_control = current_control;
				current_control = 0;
			}
			free(line);
			continue;
		}
		if (current_control)
		{
			int j=0;
			while (line[i + j])
				j++;
		}
		else
		{
			int j=0;
			while (line[i + j] && line[i + j] != '\n' && line[i + j] != ':')
				j++;
			if (line[i + j] && line[i + j] != ':')
			{
				log("Invalid control at line %i: '%s'\n", LOG_ERROR, line_idx, line);
				free(line);
				close(fd);
				return 1;
			}
			else
			{
				e_control_type	type = CONTROL_NONE;
				if (!strncmp(line + i, "Window", j - 1))
				{
					parse_window(fd, win);
					free(line);
					continue;
				}
				if (!strncmp(line + i, "Text", j - 1))
					type = CONTROL_TEXT;
				current_control = malloc(sizeof(t_control));
				if (!current_control || type == CONTROL_NONE)
				{
					if (!current_control)
						log("Could not allocate memory for control\n", LOG_ERROR);
					else
						log("Invalid control type: '%s'\n", LOG_ERROR, line + i);
					free(line);
					close(fd);
					return 1;
				}
			}
		}
		free(line);
	}
	close(fd);
	return 0;
}