#include "parsing.h"
#include "get_next_line.h"
#include "libft.h"

int	parse_source(const char *path, t_window *win)
{
	char		*line = 0;
	int			fd;
	int			i;
	int			line_nb = 0;
	t_control	*current;

	fd = open(path, O_READ);
	if (fd < 0)
		return 1;
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
		} else if (i == 0)
		{
			if (line[i] == '#') // skip comments
			{
				free(line);
				continue;
			}
		}

		free(line);
	}
	return 0;
}
