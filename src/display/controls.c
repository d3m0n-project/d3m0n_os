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
	t_window	*window = get_current_window();

	if (control->visible == 0)
		return;

	control->p_client_location = control->location;

	// move the origin of the canvas to lower for the topbar
	if (window->top_bar)
		control->p_client_location.y += TOPBAR_HEIGHT;
		
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
		draw_rect_outline(control->p_client_location.x, control->p_client_location.y, control->width, control->height, OUTLINE_COLOR);
	#endif
	draw_topbar(get_current_window()); // TODO:
}

void	draw_window(t_window *window)
{
	t_control	*current = window->controls;

	draw_rect(0, 0, window->width, window->height, window->bg_color);
	while (current)
	{
		current->p_client_location = current->location;
		if (window->top_bar)
			current->p_client_location.y += TOPBAR_HEIGHT;
		draw_control(current);
		// TODO: children
		current = current->p_next;
	}
	draw_topbar(window);
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
	control->p_client_location = (t_point){0, 0};
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
	win->controls = 0;
}