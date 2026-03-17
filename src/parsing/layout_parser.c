#include "parsing.h"
#include "get_next_line.h"

int	parse_layout(const char *path, t_window *win)
{
	int			fd = open(path, O_READ);
	char		*line;
	if (fd < 0)
	{
		log("Could not open file: %s\n", LOG_ERROR, path);
		return 1;
	}
	while ((line = get_next_line(fd)))
	{
		log("'%s'\n", LOG_INFO, line);
		free(line);
	}
	(void)win;
	close(fd);
	return 0;
}