#include "parsing.h"

int	parse_manifest(const char *path, t_window *win)
{
	int		not_found = 0;
	int		not_found2 = 0;
	char	*title = get_setting(path, "name", &not_found);
	if (!title || not_found)
	{
		log("MANIFEST: Could not find key 'name' in app manifest %s\n", LOG_ERROR, path);
		return 1;
	}
	char	*package = get_setting(path, "package", &not_found2);
	if (!package || not_found2)
	{
		free(title);
		log("MANIFEST: Could not find key 'package' in app manifest %s\n", LOG_ERROR, path);
		return 1;
	}
	if (create_window(win, (const char *)title, (const char *)package, SCREEN_WIDTH, SCREEN_HEIGHT)) // TODO: parse width and height from layout
	{
		free(title);
		free(package);
		log("MANIFEST: Could not create Window from manifest %s\n", LOG_ERROR, path);
		return 1;
	}
	free(package);
	free(title);
	return 0;
}