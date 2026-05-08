#include "scripting.h"
#include "log.h"
#include "filesystem.h"
#include "parsing.h"

void	template_load(void **args)
{
	char	*path = ((char **)args)[0];
	int x = *(int *)(args)[1];
	int y = *(int *)(args)[2];

	char	**strings = (char **)&(args[3]); // list of strings that will replace $0, $1, $2... inside of layout
	int fd = open(path, O_READ);
	if (fd < 0)
	{
		log("Could not find template file: '%s'\n", LOG_ERROR | LOG_INDENT, path);
		return;
	}
	close(fd);

	int i = 0;
	while (strings[i])
	{
		char	*arg = strings[i];
		int j = 0;

		// check args to avoid excaping from strings
		while (arg[j])
		{
			if (arg[j] == '\n' || arg[j] == '\r' || arg[j] == '"' || arg[j] == '\\')
			{
				log("Invalid template replacement string for $%i: '%s'\n", LOG_ERROR | LOG_INDENT, i, arg);
				return;
			}
			j++;
		}
		i++;
	}

	log("Loading '%s' at x:%i, y:%i\n", LOG_INFO, path, x, y);
	t_window	*current_win = get_current_window();
	if (parse_layout(path, current_win, strings, x, y))
		log("Could not load template at x:%i, y:%i\n", LOG_ERROR | LOG_INDENT, x, y);
	draw_window(current_win);
}