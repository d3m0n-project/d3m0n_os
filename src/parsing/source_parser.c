#include "parsing.h"
#include "get_next_line.h"
#include "libft.h"
#include "controls.h"

int	parse_source(const char *path, t_window *win)
{
	char		*line = 0;
	int			fd;
	int			i;
	int			line_nb = 0;
	int			event_id = 0;
	t_control	*current = 0;

	if (!win)
		return 1;
	fd = open(path, O_READ);
	if (fd < 0)
	{
		log("Could not open file: %s\n", LOG_ERROR, path);
		return 1;
	}
	// calculate the next event id
	while (!win->events[event_id].callback && event_id < MAX_WINDOW_EVENTS)
		event_id++;
	while ((line = get_next_line(fd)) != 0)
	{
		i = 0;
		line_nb++;

		// skip whitespaces
		while (line[i] && line[i] != '\n' && ((line[i] >= 9 && line[i] <= 13) || line[i] == ' '))
			i++;
		
		log("line = '%s'\n", LOG_INFO, line + i);

		if ((i > 0 && current == 0)) // invalid: defining nothing
		{
			log("Invalid line at l:%i, '%s'\n", LOG_ERROR, line_nb, line);
			free(line);
			return 1;
		}
		else if (i == 0)
		{
			if (line[i] == '#' || line[i] == '\0' || line[i] == '\n') // skip comments and empty lines
			{
				free(line);
				continue;
			}
			int j = 0;
			while (line[i+j] && line[i + j] != '.')
				j++;
			line[i+j] = 0;
			log("Assigning event to control: '%s'\n", LOG_WARNING, line[i]);
		}

		free(line);
	}
	return 0;
}
