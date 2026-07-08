#include "controls.h"

int		exec_event(t_control *control, e_event_type type, t_window *window)
{
	int	i=0;
	while (i < MAX_WINDOW_EVENTS)
	{
		if (window->events[i].type == type && control == window->events[i].affected_control) // TODO: maybe ptr eq is not very good idea
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
	static int prev_x = -1;
	static int prev_y = -1;
	static int prev_buttons = 0;
	static t_control *dragging_control = NULL;
	int dx = 0;
	int dy = 0;

	if (prev_x >= 0 && prev_y >= 0)
	{
		dx = x - prev_x;
		dy = y - prev_y;
	}

	// drag started
	if (!(prev_buttons & 1) && (button & 1))
	{
		t_control *cur = window->controls;
		//int topbar_height = window->top_bar ? TOPBAR_HEIGHT : 0;
		while (cur)
		{
			int cx = cur->location.x;
			int cy = cur->location.y;
			if (x >= cx && x <= cx + cur->width && y >= cy && y <= cy + cur->height)
			{
				if (cur->p_type == CONTROL_VSCROLL)
				{
					dragging_control = cur;
					break;
				}
			}
			cur = cur->p_next;
		}
	}

	// dragging moved
	if ((button & 1) && (prev_buttons & 1) && (dx != 0 || dy != 0) && dragging_control)
	{
		t_control *c = dragging_control;
		if (c->p_scroll_max_size.y > 0 && c->height > 0)
		{
			int new_off = c->p_scroll_offset.y - dy;
			if (new_off < 0)
				new_off = 0;
			if (new_off > c->p_scroll_max_size.y)
				new_off = c->p_scroll_max_size.y;
			c->p_scroll_offset.y = new_off;
			draw_control(c);
		}
	}

	// drag end
	if (!(button & 1) && (prev_buttons & 1))
		dragging_control = 0;

	// existing click event
	if (button == 1 && !(prev_buttons & 1))
	{
		int	i=-1;
		while (++i < MAX_WINDOW_EVENTS)
		{
			t_point top_left = window->events[i].override_trigger_corners[0];
			t_point bottom_right = window->events[i].override_trigger_corners[1];
			if (window->events[i].affected_control)
			{
				top_left = window->events[i].affected_control->p_client_location;
				bottom_right = window->events[i].affected_control->p_client_location;
				bottom_right.x += window->events[i].affected_control->width;
				bottom_right.y += window->events[i].affected_control->height;
			}
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

	prev_x = x;
	prev_y = y;
	prev_buttons = button;
}