#include "scripting.h"
#include "log.h"
#include "parsing.h"
#include "filesystem.h"
#include "package_manager.h"
#include "libft.h"

static char	*get_template_source_path(const char *layout_path, t_window *current_win)
{
	char		*package = 0;
	char		*base = 0;
	char		*tmp = 0;
	const char	*layout_segment;
	size_t		prefix_len;
	size_t		suffix_len;
	size_t		total_len;

	if (!layout_path || !current_win)
		return (0);
	if (layout_path[0] != '/')
	{
		package = get_app_path_from_package(current_win->package, PACKAGE_PATH);
		if (!package)
			return (0);
		if (!ft_strncmp(layout_path, "layouts/", 8))
		{
			base = path_add(package, "src/");
			if (!base)
			{
				free(package);
				return (0);
			}
			tmp = path_add(base, (char *)(layout_path + 8));
			free(base);
			base = tmp;
		}
		else
			base = path_add(package, (char *)layout_path);
		free(package);
		if (!base)
			return (0);
		tmp = path_add(base, ".src");
		free(base);
		return (tmp);
	}
	layout_segment = ft_strnstr(layout_path, "/source/layouts/", ft_strlen(layout_path));
	if (!layout_segment)
		return (path_add((char *)layout_path, ".src"));
	prefix_len = (size_t)(layout_segment - layout_path) + ft_strlen("/source/");
	suffix_len = ft_strlen(layout_segment + ft_strlen("/source/layouts/"));
	total_len = prefix_len + ft_strlen("src/") + suffix_len + ft_strlen(".src");
	base = ft_calloc(total_len + 1, sizeof(char));
	if (!base)
		return (0);
	ft_memcpy(base, layout_path, prefix_len);
	ft_strlcat(base, "src/", total_len + 1);
	ft_strlcat(base, layout_segment + ft_strlen("/source/layouts/"), total_len + 1);
	ft_strlcat(base, ".src", total_len + 1);
	return (base);
}

void	template_load(void **args)
{
	char *package = 0;
	char *relative_path = 0;
	char *source_path = 0;
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
	else
	{
		source_path = get_template_source_path(path, current_win);
		if (source_path && file_exists(source_path) && parse_source(source_path, current_win, strings))
			log("TEMPLATE.LOAD: Could not load template source '%s'\n", LOG_ERROR | LOG_INDENT, source_path);
	}
	if (source_path)
		free(source_path);
	if (relative_path)
		free(relative_path);
	if (package)
		free(package);
}