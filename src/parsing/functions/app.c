#include "scripting.h"
#include "controls.h"
#include "display.h"
#include "package_manager.h"
#include "log.h"
#include "libft.h"
#include "parsing.h"

static t_window	*desktop_window = 0;
static t_window	opened_window = {0};

// open an app, or a window inside of app if args[1] != 0
// code also used by fn_window_open
char*	fn_app_open(void **args)
{
	if (!desktop_window)
		desktop_window = get_current_window();
	char	*package = ((char **)args)[0];
	int		window_mode = (((char **)args)[1] != 0);
	char	*window_name = (window_mode)?((char **)args)[1]:"main";
	char	*layout_path = 0;
	char	*src_path = 0;

	layout_path = get_app_path_from_package(package, PACKAGE_LAYOUT_PATH);
	src_path = get_app_path_from_package(package, PACKAGE_SOURCE_PATH);
	if (!layout_path || !src_path)
	{
		log("%s: Could not allocate window paths\n", LOG_ERROR | LOG_INDENT, (window_mode)?"WINDOW:OPEN":"APP.OPEN");
		if (layout_path)
			free(layout_path);
		if (src_path)
			free(src_path);
		return 0;
	}

	// create path with the window name
	char	*new_layout_path = path_add(layout_path, window_name);
	char	*new_src_path = path_add(src_path, window_name);
	free(src_path);
	free(layout_path);
	if (!new_layout_path || !new_src_path)
	{
		if (new_layout_path)
			free(new_layout_path);
		if (new_src_path)
			free(new_src_path);
		return 0;
	}
	layout_path = ft_strjoin(new_layout_path, ".layout");
	src_path = ft_strjoin(new_src_path, ".src"); // path/to/abc.layout => path/to/abc.src
	free(new_src_path);
	free(new_layout_path);
	if (!src_path || !layout_path)
	{
		if (layout_path)
			free(layout_path);
		if (src_path)
			free(src_path);
		return 0;
	}
	

	char	*manifest = get_app_path_from_package(package, PACKAGE_MANIFEST);
	if (!manifest)
	{
		free(src_path);
		free(layout_path);
		log("%s: Could not allocate app manifest path\n", LOG_ERROR | LOG_INDENT, (window_mode)?"WINDOW:OPEN":"APP.OPEN");
		return 0;
	}
	
	cleanup_window(&opened_window); // already initialized, clear window

	if (parse_manifest(manifest, &opened_window))
	{
		free(layout_path);
		free(src_path);
		free(manifest);
		log("%s: Could not parse manifest at '%s' for package: '%s'\n", LOG_ERROR | LOG_INDENT, (window_mode)?"WINDOW:OPEN":"APP.OPEN", manifest, package);
		return 0;
	}
	if (parse_layout(layout_path, &opened_window, 0, 0, 0, 0))
	{
		free(layout_path);
		free(src_path);
		free(manifest);
		log("%s: Could not parse layout for package: '%s'\n", LOG_ERROR | LOG_INDENT, (window_mode)?"WINDOW:OPEN":"APP.OPEN", package);
		return 0;
	}
	if (parse_source(src_path, &opened_window, 0))
	{
		free(layout_path);
		free(src_path);
		free(manifest);
		log("%s: Could not parse layout for package: '%s'\n", LOG_ERROR | LOG_INDENT, (window_mode)?"WINDOW:OPEN":"APP.OPEN", package);
		return 0;
	}
	if (!window_mode)
	{
		// clear stack history
		for (int i=0; i<HISTORY_STACK_SIZE; i++)
		{
			if (window_stack[i].package)
				free(window_stack[i].package);
			if (window_stack[i].window)
				free(window_stack[i].window);
			ft_memset(&window_stack[i], 0, sizeof(t_window_stack));
		}
		window_stack[0].package = ft_strdup(package);
		window_stack[0].window = ft_strdup(window_name);
	}
	ft_strlcpy(opened_window.layout_name, window_name, 128);
	set_current_window(&opened_window);
	exec_event(0, EVENT_ON_CREATE, &opened_window);
	free(layout_path);
	free(src_path);
	free(manifest);
	return 0;
}

char*	fn_app_exit(void **args)
{
	(void)args;
	t_window	*current_window = get_current_window();
	if (!desktop_window)
	{
		log("APP.EXIT: Desktop Window not saved yet\n", LOG_WARNING);
		return 0;
	}
	if (current_window == desktop_window)
	{
		log("APP.EXIT: Cant quit from launcher app\n", LOG_WARNING);
		return 0;
	}
	cleanup_window(current_window);
	set_current_window(desktop_window);
	return 0;
}