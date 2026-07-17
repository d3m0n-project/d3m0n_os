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
	ft_memset(out->layout_name, 0, 128);
	ft_memset(out->variables, 0, sizeof(t_script_var) * MAX_WINDOW_VARS);
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

static int	get_percentage(int value, int max)
{
	if (max <= 0)
		return (0);

	return ((long long)value * max) / 100;
}

void	compute_control_layout(t_control *control, t_control *parent, t_point offset)
{
	t_window	*window;
	int			parent_width;
	int			parent_height;
	int			parent_x;
	int			parent_y;

	window = get_current_window();
	if (!window || !control)
		return;

	parent_width = parent ? parent->p_client_size.x : window->width;
	parent_height = parent ? parent->p_client_size.y : window->height;

	parent_x = parent ? parent->p_client_location.x : 0;
	parent_y = parent ? parent->p_client_location.y : 0;

	// size
	control->p_client_size.x = control->p_width_is_percent ? get_percentage(control->width, parent_width) : control->width;
	control->p_client_size.y = control->p_height_is_percent ? get_percentage(control->height, parent_height) : control->height;

	if (parent && (parent->p_type == CONTROL_VSCROLL || parent->p_type == CONTROL_HSCROLL))
	{
		if (parent->p_type == CONTROL_VSCROLL && control->p_width_is_percent)
			control->p_client_size.x -= SCROLLBAR_SIZE;
		else if (control->p_height_is_percent)
			control->p_client_size.y -= SCROLLBAR_SIZE;
	}


	// location
	control->p_client_location.x = parent_x + offset.x + (control->p_x_is_percent ? get_percentage(control->location.x, parent_width) : control->location.x);
	control->p_client_location.y = parent_y + offset.y + (control->p_y_is_percent ? get_percentage(control->location.y, parent_height) : control->location.y);

	if (!parent && window->top_bar)
		control->p_client_location.y += TOPBAR_HEIGHT;
}

void	draw_control(t_control *control)
{
	t_window	*window;
	
	window = get_current_window();
	if (!control || !control->visible)
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
		draw_rect_outline(control->p_client_location.x, control->p_client_location.y, control->p_client_size.x, control->p_client_size.y, OUTLINE_COLOR);
	#endif

	draw_topbar(window);
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
		compute_control_layout(current, 0, (t_point){0, 0});
		draw_control(current);
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