#include "scripting.h"
#include "log.h"
#include "parsing.h"
#include "filesystem.h"
#include "package_manager.h"

void	template_load(void **args)
{
	char *package = 0;
	char *relative_path = 0;
	char	*path = ((char **)args)[0];
	int x = *(int *)(args)[1];
	int y = *(int *)(args)[2];

	t_window	*current_win = get_current_window();
	char	**strings = (char **)&(args[3]); // list of strings that will replace $0, $1, $2... inside of layout

	if (!file_exists(path))
	{
		package = get_app_path_from_package(current_win->package, PACKAGE_PATH);
		if (!package)
		{
			log("TEMPLATE.LOAD: Could not allocate memory for app path\n", LOG_ERROR | LOG_INDENT);
			return;
		}
		relative_path = path_add(package, path);
		if (!relative_path)
		{
			free(package);
			log("TEMPLATE.LOAD: Could not allocate memory for relative app path\n", LOG_ERROR | LOG_INDENT);
			return;
		}
		if (!file_exists(relative_path))
		{
			log("TEMPLATE.LOAD: Could not find template file: '%s'\n", LOG_ERROR | LOG_INDENT, path);
			free(package);
			free(relative_path);
			return;
		}
	}

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
				log("TEMPLATE.LOAD: Invalid template replacement string for $%i: '%s'\n", LOG_ERROR | LOG_INDENT, i, arg);
				return;
			}
			j++;
		}
		i++;
	}

	log("Loading '%s' at x:%i, y:%i\n", LOG_INFO, path, x, y);
	if (relative_path)
		path = relative_path;
	if (parse_layout(path, current_win, strings, x, y))
		log("TEMPLATE.LOAD: Could not load template at x:%i, y:%i\n", LOG_ERROR | LOG_INDENT, x, y);
	if (relative_path)
		free(relative_path);
	if (package)
		free(package);
}