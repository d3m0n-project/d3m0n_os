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
	t_control	*current;

	fd = open(path, O_READ);
	if (fd < 0)
		return 1;
	// calculate the next event id
	while (!win->events[event_id].callback && event_id < MAX_WINDOW_EVENTS)
		event_id++;
	while (!(line = get_next_line(fd)))
	{
		i = 0;
		line_nb++;

		// skip whitespaces
		while (line[i] && ((line[i] >= 9 && line[i] <= 13) || line[i] == ' '))
			i++;

		if ((i > 0 && current == 0)) // invalid: defining nothing
		{
			log("Invalid line at l:%i, '%s'\n", LOG_ERROR, line_nb, line);
			free(line);
			return 1;
		}
		else if (i == 0)
		{
			if (line[i] == '#' || line[i] == '\0') // skip comments and empty lines
			{
				free(line);
				continue;
			}
		}

		free(line);
	}
	return 0;
}
