#include "controls_graphics.h"

void	ctrl_draw_rect(t_control	*control)
{
	draw_rect(control->p_client_location.x, control->p_client_location.y, control->width, control->height, control->bg_color);

	if (control->children)
	{
		t_control *child = control->children;
		while (child)
		{
			if (child == control) // avoid infinite loop
				break;
			
			// apply scroll to children too
			child->p_scroll_offset = control->p_scroll_offset;

			// only draw if visible in the area
			if (child->p_client_location.y + child->height >= control->p_client_location.y && child->p_client_location.y <= control->p_client_location.y + control->height)
				draw_control(child, (t_point){0, -control->p_scroll_offset.y});

			child = child->p_next;
		}
	}
}