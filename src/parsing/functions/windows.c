#include "scripting.h"
#include "controls.h"
#include "filesystem.h"
#include "package_manager.h"
#include "libft.h"
#include "memory.h"

t_window_stack	window_stack[HISTORY_STACK_SIZE + 1] = {0};
int				window_stack_top = 0;

char *fn_window_open(void **args)
{
	t_window	*current = get_current_window();
	char		*layout = ((char **)args)[0];

	if (window_stack_top >= HISTORY_STACK_SIZE)
	{
		log("WINDOW.OPEN: Window history stack is full (max=%i)\n", LOG_WARNING, HISTORY_STACK_SIZE);
		return 0;
	}

	window_stack[window_stack_top].package = ft_strdup(current->package);
	window_stack[window_stack_top].window = ft_strdup(current->layout_name); // current window
	window_stack_top++;

	char	*app_open_args[] = {current->package, layout, 0};
	return fn_app_open((void **)app_open_args);
}

char	*fn_window_go_back(void **args)
{
	(void)args;
	if (window_stack_top == 0)
	{
		log("WINDOW.GOBACK: History is empty\n", LOG_WARNING);
		return (0);
	}

	window_stack_top--;

	char *package = window_stack[window_stack_top].package;
	char *window  = window_stack[window_stack_top].window;

	char *app_open_args[] = {package, window, 0};
	fn_app_open((void **)app_open_args);

	free(package);
	free(window);

	window_stack[window_stack_top].package = 0;
	window_stack[window_stack_top].window = 0;

	return 0;
}