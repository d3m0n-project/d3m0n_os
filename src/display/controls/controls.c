#include "controls.h"
#include "display.h"
#include "log.h"
#include "memory.h"

int	create_window(t_window *out, const char *title, int w, int h)
{
	int	i = 0;
	if (!title)
		return 1;
	if (w < 0 || h < 0 || w > SCREEN_WIDTH || h > SCREEN_HEIGHT)
		return 1;
	while (title[i] && i < 24)
	{
		out->title[i] = ((char *)title)[i];
		i++;
	}
	if (title[i])
		return 1;
	out->title[i] = '\0';
	out->width = w;
	out->height = h;
	out->top_bar = 1;
	out->controls = 0;
	return 0;
}

void	add_control(t_window *to, t_control *control)
{
	t_control	*current = to->controls;

	control->p_next = 0;
	if (current == 0)
	{
		to->controls = control;
		current = to->controls;
	}
	else
	{
		while (current)
			current = current->p_next;
		current->p_next = control;
	}
	log("Added control: '%s'\n", LOG_SUCCESS, current->name);
}

void	draw_control(t_control *control)
{
	switch (control->p_type)
	{
	case CONTROL_RECT:
		draw_rect(control->location.x, control->location.y, control->width, control->height, control->bg_color);
		break;
	
	default:
		log("Unknown control type: id=%i\n", LOG_WARNING, control->p_type);
		break;
	}
}

void	draw_window(t_window *window)
{
	t_control	*current = window->controls;

	draw_rect(0, 0, window->width, window->height, window->bg_color);
	while (current)
	{
		draw_control(current);
		// TODO: children
		current = current->p_next;
	}
	if (window->top_bar)
		draw_rect(0, 0, window->width, 20, DISPLAY_COLORS[WHITE]);
}

void	init_control(t_control *control, const char *name, e_control_type type)
{
	int	i = 0;
	memset(control, 0, sizeof(t_control));
	while (name[i] && i < 24)
	{
		control->name[i] = ((char *)name)[i];
		i++;
	}
	control->enabled = 1;
	control->p_type = type;
	control->visible = 1;
}