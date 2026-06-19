#include "parsing.h"

int	parse_manifest(const char *path, t_window *win)
{
	char	*title = get_setting(path, "name");
	if (!title)
	{
		log("MANIFEST: Could not find key 'name' in app manifest %s\n", LOG_ERROR, path);
		return 1;
	}
	if (create_window(win, (const char *)title, SCREEN_WIDTH, SCREEN_HEIGHT)) // TODO: parse width and height from layout
	{
		free(title);
		log("MANIFEST: Could not create Window from manifest %s\n", LOG_ERROR, path);
		return 1;
	}
	free(title);
	return 0;
}