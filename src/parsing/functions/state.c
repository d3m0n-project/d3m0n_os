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

	if (!lp_apply_control_attr(control, key, value))
		log("STATE.SET: Invalid control field: '%s' value: '%s'\n", LOG_ERROR, key, value);
	else
	{
		compute_control_layout(control, control->p_parent, (t_point){0, 0});
		draw_control(control);
	}
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
	else if (!ft_strcmp(key, "width"))											return ft_itoa(control->width);
	else if (!ft_strcmp(key, "height"))											return ft_itoa(control->height);
	else if (!ft_strcmp(key, "x") || !ft_strcmp(key, "location.x"))				return ft_itoa(control->p_client_location.x);
	else if (!ft_strcmp(key, "y") || !ft_strcmp(key, "location.y"))				return ft_itoa(control->p_client_location.y);

	else if (!ft_strcmp(key, "margin_top"))										return ft_itoa(control->margin_top);
	else if (!ft_strcmp(key, "margin_left"))									return ft_itoa(control->margin_left);
	else if (!ft_strcmp(key, "margin_right"))									return ft_itoa(control->margin_right);
	else if (!ft_strcmp(key, "margin_bottom"))									return ft_itoa(control->margin_bottom);

	else if (!ft_strcmp(key, "font_size"))										return ft_itoa(control->font_size);
	else if (!ft_strcmp(key, "min"))											return ft_itoa(control->min);
	else if (!ft_strcmp(key, "max"))											return ft_itoa(control->max);
	else if (!ft_strcmp(key, "value"))											return ft_itoa(control->value);
	else if (!ft_strcmp(key, "radius"))											return ft_itoa(control->radius);
	
	else if (!ft_strcmp(key, "mode"))											return ft_itoa(control->mode); // TODO: return mode name instead (need str cmp)?
	else if (!ft_strcmp(key, "type"))											return ft_itoa(control->type); // TODO: same
	else if (!ft_strcmp(key, "text_align"))										return ft_itoa(control->text_align); // TODO: same
	else if (!ft_strcmp(key, "color") || !ft_strcmp(key, "bg_color"))			// TODO: same thing
	{
		if (key[0] == 'b')
			return ft_itoa(control->bg_color);
		return ft_itoa(control->color); 
	}
	log("STATE.GET: Unknown field for a control: '%s'\n", LOG_WARNING, key);
	return 0;
}