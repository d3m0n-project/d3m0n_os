#include "controls.h"

int		exec_event(int control_id, e_event_type type, t_window *window)
{
	int	i=0;
	while (i < MAX_WINDOW_EVENTS)
	{
		if (window->events[i].type == type && control_id == window->events[i].affected_control_id)
		{
			exec_script(window->events[i].script);
			return 0;
		}
		i++;
	}
	return 1;
}

void	handle_click(int x, int y, int button, t_window *window)
{
	if (button == 1) // left click
	{
		int	i=-1;
		while (++i < MAX_WINDOW_EVENTS)
		{
			t_point top_left = window->events[i].trigger_corners[0];
			t_point bottom_right = window->events[i].trigger_corners[1];
			if (window->events[i].type == EVENT_ON_CLICK)
			{
				if (x < top_left.x || x > bottom_right.x)
					continue;
				if (y < top_left.y || y > bottom_right.y)
					continue;
				exec_script(window->events[i].script);
				return;
			}
		}
	}
}