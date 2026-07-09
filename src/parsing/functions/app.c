#include "scripting.h"
#include "controls.h"
#include "display.h"
#include "package_manager.h"
#include "log.h"
#include "parsing.h"

static t_window	*desktop_window = 0;
static t_window	opened_window = {0};

char*	fn_app_open(void **args)
{
	if (!desktop_window)
		desktop_window = get_current_window();
	char	*package = ((char **)args)[0];
	char	*layout_path = get_app_path_from_package(package, PACKAGE_MAIN_LAYOUT);
	if (!layout_path)
	{
		log("APP.OPEN: Could not allocate main layout path\n", LOG_ERROR | LOG_INDENT);
		return 0;
	}
	char	*src_path = get_app_path_from_package(package, PACKAGE_MAIN_SOURCE);
	if (!src_path)
	{
		free(layout_path);
		log("APP.OPEN: Could not allocate main source path\n", LOG_ERROR | LOG_INDENT);
		return 0;
	}
	char	*manifest = get_app_path_from_package(package, PACKAGE_MANIFEST);
	if (!manifest)
	{
		free(src_path);
		free(layout_path);
		log("APP.OPEN: Could not allocate app manifest path\n", LOG_ERROR | LOG_INDENT);
		return 0;
	}
	
	if (opened_window.controls) // already initialized, clear controls
		free_controls(&opened_window);
	if (opened_window.events[0].script) // clear events
	{
		for (int i=0; i<MAX_WINDOW_EVENTS; i++)
		{
			if (opened_window.events[i].script)
				free(opened_window.events[i].script);
			opened_window.events[i].script = 0;
		}
	}
	if (parse_manifest(manifest, &opened_window))
	{
		free(layout_path);
		free(src_path);
		free(manifest);
		log("APP.OPEN: Could not parse manifest at '%s' for package: '%s'\n", LOG_ERROR | LOG_INDENT, manifest ,package);
		return 0;
	}
	if (parse_layout(layout_path, &opened_window, 0, 0, 0))
	{
		free(layout_path);
		free(src_path);
		free(manifest);
		log("APP.OPEN: Could not parse layout for package: '%s'\n", LOG_ERROR | LOG_INDENT ,package);
		return 0;
	}
	if (parse_source(src_path, &opened_window, 0))
	{
		free(layout_path);
		free(src_path);
		free(manifest);
		log("APP.OPEN: Could not parse layout for package: '%s'\n", LOG_ERROR | LOG_INDENT ,package);
		return 0;
	}
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
	cleanup_window(current_window);
	set_current_window(desktop_window);
	return 0;
}