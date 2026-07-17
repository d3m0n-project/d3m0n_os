#include "controls.h"
#include "memory.h"

void	cleanup_window(t_window *win)
{
	if (!win)
		return;
	for (int i=0; i<MAX_WINDOW_EVENTS; i++)
	{
		if (win->events[i].type == EVENT_UNDEFINED)
			continue;
		if (win->events[i].script)
			free_script(win->events[i].script);
		win->events[i].script = 0;
	}
	for (int i=0; i < MAX_WINDOW_VARS; i++)
	{
		if (win->variables[i].used && win->variables[i].type == SCRIPT_VAR_STR && win->variables[i].sval)
			free(win->variables[i].sval);
		win->variables[i].used = 0;
		win->variables[i].type = SCRIPT_VAR_NONE;
		win->variables[i].sval = 0;
		win->variables[i].ival = 0;
		i++;
	}
	if (win->controls)
		free_controls(win);
	if (win->bmp_cache)
	{
		t_bmp_cache *current = win->bmp_cache;
		while (current)
		{
			if (current->texture)
				free_bmp_texture(current->texture);
			current->texture = 0;
			t_bmp_cache *next = current->next;
			free(current);
			current = next;
		}
		win->bmp_cache = 0;
	}
}