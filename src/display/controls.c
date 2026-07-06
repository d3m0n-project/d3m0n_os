#include "controls.h"
#include "display.h"
#include "log.h"
#include "memory.h"
#include "libft.h"
#include "time.h"
#include "scripting.h"
#include "controls_graphics.h"

int	create_window(t_window *out, const char *title, const char *package, int w, int h)
{
	int	i = 0;
	log("Creating a %ix%i Window title: '%s'...\n", LOG_INFO, w, h, title);
	if (!title)
	{
		log("Window title cant be null\n", LOG_ERROR | LOG_INDENT);
		return 1;
	}
	if (w < 0 || h < 0 || w > SCREEN_WIDTH || h > SCREEN_HEIGHT)
	{
		log("Maximum Window size is %ix%i, got %ix%i\n", LOG_ERROR | LOG_INDENT, SCREEN_WIDTH, SCREEN_HEIGHT, w, h);
		return 1;
	}
	while (title[i] && i < 24)
	{
		out->title[i] = ((char *)title)[i];
		i++;
	}
	if (title[i])
	{
		log("Invalid given window title length: max 24 chars\n", LOG_ERROR | LOG_INDENT);
		return 1;
	}
	out->title[i] = '\0';

	i = 0;
	while (package[i] && i < 127)
	{
		out->package[i] = ((char *)package)[i];
		i++;
	}
	if (package[i])
	{
		log("Invalid given window package length: max 127 chars\n", LOG_ERROR | LOG_INDENT);
		return 1;
	}
	out->package[i] = '\0';

	out->width = w;
	out->height = h;
	out->is_launcher = 0;
	out->top_bar = 1; // TODO: parse manifest
	out->controls = 0;
	ft_memset(out->events, 0, MAX_WINDOW_EVENTS * sizeof(t_event));
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
	if (control->visible == 0)
		return;
	switch (control->p_type)
	{
		case CONTROL_TEXTBOX:		ctrl_draw_textbox(control);		break;
		case CONTROL_LISTVIEW:		ctrl_draw_listview(control);	break;
		case CONTROL_PROGRESSBAR:	ctrl_draw_progressbar(control);	break;
		case CONTROL_CHECKBOX:		ctrl_draw_checkbox(control);	break;
		case CONTROL_RADIOBUTTON:	ctrl_draw_radiobutton(control);	break;
		case CONTROL_WEBVIEW:		ctrl_draw_webview(control);		break;
		case CONTROL_RECT:			ctrl_draw_rect(control);		break;
		case CONTROL_SWITCH:		ctrl_draw_switch(control);		break;
		case CONTROL_TEXT:			ctrl_draw_text(control);		break;
		case CONTROL_IMAGE:			ctrl_draw_image(control);		break;
		case CONTROL_ROUNDBUTTON:	ctrl_draw_roundbutton(control);	break;
		case CONTROL_BUTTON:		ctrl_draw_button(control);		break;
		case CONTROL_HSCROLL:		ctrl_draw_hscroll(control);		break;
		case CONTROL_VSCROLL:		ctrl_draw_vscroll(control);		break;
		default:
			log("Unknown control type: id=%i\n", LOG_WARNING, control->p_type);
			break;
	}
	#if DEBUG_OUTLINE == 1
		draw_rect_outline(control->location.x, control->location.y, control->width, control->height, OUTLINE_COLOR);
	#endif
}

void	draw_window(t_window *window)
{
	t_control	*current = window->controls;
	t_conf		*conf = get_config();
	int			topbar_height = 20;

	draw_rect(0, 0, window->width, window->height, window->bg_color);
	while (current)
	{
		// move the origin of the canvas to lower for the topbar
		if (window->top_bar)
			current->location.y += topbar_height;
		draw_control(current);
		if (window->top_bar)
			current->location.y -= topbar_height;
		// TODO: children
		current = current->p_next;
	}
	if (window->top_bar)
	{
		draw_rect(0, 0, window->width, topbar_height, DISPLAY_COLORS[WHITE]);

		size_t	time = (time_us() % (1000 * 60 * 24 * 1000)) / (1000*1000*60); // minute of the day
		int		hours = time / 60;
		int		minutes = time % 60;

		char	clock[9] = "00:00 AM";
		if (conf->time_mode == 1) // 24h clock
			clock[5] = '\0';
		else if (hours >= 12)
		{
			clock[6] = 'P';
			if (hours > 12)
				hours %= 12;
		}
		clock[0] = '0' + (hours / 10);
		clock[1] = '0' + hours % 10;

		clock[3] = '0' + (minutes / 10);
		clock[4] = '0' + minutes % 10;

		draw_text(2, 2, 8, 16, clock, DISPLAY_COLORS[MAGENTA], 0); // TODO: change topbar
		if (!window->is_launcher)
			draw_text(66, 2, 8, 16, window->title, DISPLAY_COLORS[MAGENTA], 0);
	}
	if (!window->is_launcher)
	{
		int cross_x = SCREEN_WIDTH - 15;
		int cross_y = 5;
		int cross_s = 10;
		draw_text(cross_x, cross_y, cross_s, cross_s, "X", DISPLAY_COLORS[RED], 0); // draw exit icon
		// add close event if not added yet
		if (window->events[0].type == EVENT_UNDEFINED)
		{
			window->events[0].script = init_script(0);
			if (!window->events[0].script)
			{
				panic("Could not allocate the app close cross script\n");
				return;
			}
			window->events[0].type = EVENT_ON_CLICK;
			window->events[0].script->func = fn_app_exit;
			window->events[0].script->args = 0;
			window->events[0].script->next = 0;
			window->events[0].trigger_corners[0] = (t_point){.x=cross_x, .y=cross_y};
			window->events[0].trigger_corners[1] = (t_point){.x=cross_x+cross_s, .y=cross_y+cross_s};
		}
	}
}

void	init_control(t_control *control, const char *name, e_control_type type)
{
	int	i = 0;
	ft_memset(control, 0, sizeof(t_control));
	while (name[i] && i < 24)
	{
		control->name[i] = ((char *)name)[i];
		i++;
	}
	control->enabled = 1;
	control->p_type = type;
	control->visible = 1;
}

void	free_controls(t_window	*win)
{
	if (!win)
		return;
	t_control	*current = win->controls;
	while (current)
	{
		t_control *next = current->p_next;
		free(current);
		current = next;
	}
}