#include "parsing.h"
#include "get_next_line.h"

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
		int i = 0;
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (line[i] == '#')
		{
			free(line);
			continue;
		}
		if (line[i] == '\n' || line[i] == '\0')
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
			if (line[i + j] == '\n' || line[i + j] == '\0')
			{
				log("Invalid layout control entry l.%i: '%s'\n", LOG_ERROR, line_idx, line);
				free(line);
				close(fd);
				return 1;
			}
			else
			{
				current_control = malloc(sizeof(t_control));
				if (!current_control)
				{
					log("Could not allocate memory for control\n", LOG_ERROR);
					free(line);
					close(fd);
					return 1;
				}
			}
		}
		free(line);
		line_idx++;
	}
	close(fd);
	return 0;
}