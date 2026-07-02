#include "parsing.h"
#include "get_next_line.h"

int	parse_manifest(const char *path, t_window *win)
{
	int		not_found = 0;
	char	*title = get_setting(path, "name", &not_found);
	if (!title || not_found)
	{
		log("MANIFEST: Could not find key 'name' in app manifest %s\n", LOG_ERROR, path);
		return 1;
	}
	//int fd = open("")
	char	*package = "com.4re5.d3m0n.system.launcher";//get_next_line(fd); // TODO: Do dynamic package
	if (create_window(win, (const char *)title, (const char *)package, SCREEN_WIDTH, SCREEN_HEIGHT)) // TODO: parse width and height from layout
	{
		free(title);
		log("MANIFEST: Could not create Window from manifest %s\n", LOG_ERROR, path);
		return 1;
	}
	free(title);
	return 0;
}