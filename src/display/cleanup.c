#include "controls.h"

void	cleanup_window(t_window *win)
{
	if (!win)
		return;
	for (int i=0; i<MAX_WINDOW_EVENTS; i++)
	{
		if (win->events[i].type == EVENT_UNDEFINED)
			continue;
		free_script(win->events[i].script);
	}
	free_controls(win);
}