#include "controls.h"
#include "log.h"
#include "controls_graphics.h"

int		exec_event(t_control *control, e_event_type type, t_window *window)
{
	int	i=0;
	while (i < MAX_WINDOW_EVENTS)
	{
		if (window->events[i].type == type && control == window->events[i].affected_control) // TODO: maybe ptr eq is not very good idea
		{
			exec_script(window->events[i].script, window);
			return 0;
		}
		i++;
	}
	return 1;
}

static t_control	*find_clicked_control(t_control *control, t_window *win, int x, int y)
{
	while (control)
	{
		if (control->children)
		{
			t_control *hit = find_clicked_control(control->children, win, x, y);
			if (hit)
				return hit;
		}

		if (!control->p_is_reactive) // if not reactive, not special script associated
		{
			control = control->p_next;
			continue;
		}

		if (control->enabled)
		{
			int cx = control->p_client_location.x;
			int cy = control->p_client_location.y;

			if (x >= cx && x <= cx + control->p_client_size.x && y >= cy && y <= cy + control->p_client_size.y)
				return control;
		}
		control = control->p_next;
	}
	return 0;
}

void	handle_click(int x, int y, int button, t_window *window)
{
	static int			prev_x = -1;
	static int			prev_y = -1;
	static int			prev_buttons = 0;
	static t_control	*dragging_control = 0;
	int					dx = 0;
	int					dy = 0;

	if (prev_x >= 0 && prev_y >= 0)
	{
		dx = x - prev_x;
		dy = y - prev_y;
	}

	// drag started
	if (!(prev_buttons & 1) && (button & 1))
	{
		t_control *cur = find_clicked_control(window->controls, window, x, y);

		if (cur)
		{
			if (cur->p_type == CONTROL_VSCROLL || cur->p_type == CONTROL_HSCROLL) // TODO: Hscroll
				dragging_control = cur;
			else if (cur->p_type == CONTROL_SWITCH || cur->p_type == CONTROL_CHECKBOX || cur->p_type == CONTROL_RADIOBUTTON)
			{
				cur->checked = !cur->checked;
				compute_control_layout(cur, cur->p_parent, (t_point){0, 0});
				draw_control(cur);
			}
			else if (cur->p_type == CONTROL_PROGRESSBAR)
			{
				int total_size = cur->p_client_size.x - (PROGRESSBAR_PADDING * 2);
				if (total_size <= 0)
				{
					log("PROGRESSBAR '%s' size is too short to handle it correctly\n", LOG_WARNING | LOG_INDENT);
					return;
				}

				int local_x = x - (cur->p_client_location.x + PROGRESSBAR_PADDING);
				// clamp
				if (local_x < 0)
					local_x = 0;
				if (local_x > total_size)
					local_x = total_size;

				float percentage = (float)local_x / (float)total_size;
				cur->value = cur->min + (cur->max - cur->min) * percentage;
				compute_control_layout(cur, cur->p_parent, (t_point){0, -cur->p_scroll_offset.y});
				draw_control(cur);
			}
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
			compute_control_layout(c, c->p_parent, (t_point){0, 0});
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
				bottom_right.x += window->events[i].affected_control->p_client_size.x;
				bottom_right.y += window->events[i].affected_control->p_client_size.y;
			}
			if (window->events[i].type == EVENT_ON_CLICK)
			{
				if (x < top_left.x || x > bottom_right.x)
					continue;
				if (y < top_left.y || y > bottom_right.y)
					continue;
				exec_script(window->events[i].script, window);
				return;
			}
		}
	}

	prev_x = x;
	prev_y = y;
	prev_buttons = button;
}