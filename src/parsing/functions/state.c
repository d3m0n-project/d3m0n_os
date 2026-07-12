#include "scripting.h"
#include "controls.h"
#include "parsing.h"
#include "libft.h"

char	*fn_state_set(void **args)
{
	t_window	*current_window = get_current_window();
	char		*control_name = ((char **)args)[0];
	char		*key = ((char **)args)[1];
	char		*value = ((char **)args)[2];
	t_control	*control = lp_find_control_by_name(current_window->controls, control_name);

	if (!control)
	{
		log("STATE.SET: Could not find control with name: '%s'\n", LOG_ERROR, control_name);
		return 0;
	}

	t_control	*parent = 0;
	if (!lp_apply_control_attr(control, current_window, parent, key, value)) // TODO: find parent
		log("STATE.SET: Invalid control field: '%s' value: '%s'\n", LOG_ERROR, key, value);
	else
		draw_control(control);
	return 0;
}

char	*fn_state_get(void **args)
{
	t_window	*current_window = get_current_window();
	char		*control_name = ((char **)args)[0];
	char		*key = ((char **)args)[1];

	t_control	*control = lp_find_control_by_name(current_window->controls, control_name);
	if (!control)
	{
		log("STATE.GET: Could not find control with name: '%s'\n", LOG_ERROR, control_name);
		return 0;
	}

	if (!ft_strcmp(key, "name"))												return ft_strdup(control->name);
	else if (!ft_strcmp(key, "content") || !ft_strcmp(key, "text"))				return ft_strdup(control->content);
	else if (!ft_strcmp(key, "visible"))										return (control->visible)?"1":"0";
	else if (!ft_strcmp(key, "enabled"))										return (control->enabled)?"1":"0";
	else if (!ft_strcmp(key, "checked"))										return (control->checked)?"1":"0";
	else if (!ft_strcmp(key, "bar"))											return (control->bar)?"1":"0";
	else if (!ft_strcmp(key, "src") || !ft_strcmp(key, "image"))				return ft_strdup(control->image);

	// TODO: last fields that require mallocs
	//else if (!ft_strcmp(key, "width"))
	//{
	//	control->width = lp_parse_percent(value, lp_get_relative_base(win, parent, 1));
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "height"))
	//{
	//	control->height = lp_parse_percent(value, lp_get_relative_base(win, parent, 0));
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "x"))
	//{
	//	control->location.x = lp_parse_percent(value, lp_get_relative_base(win, parent, 1));
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "y"))
	//{
	//	control->location.y = lp_parse_percent(value, lp_get_relative_base(win, parent, 0));
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "color"))
	//{
	//	control->color = lp_parse_color(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "bg_color"))
	//{
	//	control->bg_color = lp_parse_color(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "margin_top"))
	//{
	//	control->margin_top = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "margin_left"))
	//{
	//	control->margin_left = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "margin_right"))
	//{
	//	control->margin_right = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "margin_bottom"))
	//{
	//	control->margin_bottom = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "location"))
	//{
	//	int align;

	//	if (lp_parse_location_xy(value, &control->location, win ? win->width : SCREEN_WIDTH, win ? win->height : SCREEN_HEIGHT))
	//		return 1;
	//	align = lp_parse_align(value);
	//	if (align == 0)
	//		return -1;
	//	control->p_anchor_location = (e_control_anchor)align;
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "text_align"))
	//{
	//	control->text_align = lp_parse_align(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "font_size"))
	//{
	//	control->font_size = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "min"))
	//{
	//	control->min = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "max"))
	//{
	//	control->max = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "value"))
	//{
	//	control->value = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "radius"))
	//{
	//	control->radius = ft_atoi(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "mode"))
	//{
	//	control->mode = lp_parse_image_mode(value);
	//	return 1;
	//}
	//else if (!ft_strcmp(key, "type"))
	//{
	//	control->type = lp_parse_textbox_type(value);
	//	return 1;
	//}



	return 0;
}