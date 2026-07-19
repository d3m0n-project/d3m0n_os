#include "controls_graphics.h"
#include "math.h"
#include "controls.h"

#include "log.h"

void	ctrl_draw_vscroll(t_control *control)
{
	t_control	*child;
	int			scroll_thumb_offset;
	int			thumb_height;

	draw_rect(control->p_client_location.x, control->p_client_location.y, control->p_client_size.x, control->p_client_size.y, control->bg_color);

	child = control->children;
	while (child)
	{
		if (child == control)
			break;

		child->p_scroll_offset = control->p_scroll_offset;
		compute_control_layout(child, control, (t_point){0, -child->p_scroll_offset.y});


		put_pixel(control->p_client_location.x, control->p_client_location.y, DISPLAY_COLORS[YELLOW]);
		// check if child is in area
		if ((child->p_client_location.y >= control->p_client_location.y && child->p_client_location.y <= control->p_client_location.y + control->p_client_size.y)
			&& (child->p_client_location.x >= control->p_client_location.x && child->p_client_location.x <= control->p_client_location.x + control->p_client_size.x))
		{
			draw_control(child);
			put_pixel(child->p_client_location.x, child->p_client_location.y, DISPLAY_COLORS[RED]);
		}

		child = child->p_next;
	}

	if (!control->bar)
		return;

	draw_rect(control->p_client_location.x + control->p_client_size.x - SCROLLBAR_SIZE, control->p_client_location.y, SCROLLBAR_SIZE, control->p_client_size.y, DISPLAY_COLORS[GREY]);

	if (control->p_scroll_max_size.y > 0)
	{
		thumb_height = (control->p_client_size.y * control->p_client_size.y) / (control->p_client_size.y + control->p_scroll_max_size.y);

		if (thumb_height < SCROLLBAR_THUMB_MIN_HEIGHT)
			thumb_height = SCROLLBAR_THUMB_MIN_HEIGHT;

		scroll_thumb_offset = (control->p_scroll_offset.y * (control->p_client_size.y - thumb_height)) / control->p_scroll_max_size.y;
	}
	else
	{
		thumb_height = control->p_client_size.y;
		scroll_thumb_offset = 0;
	}

	draw_rect(control->p_client_location.x + control->p_client_size.x - SCROLLBAR_SIZE, control->p_client_location.y + scroll_thumb_offset, SCROLLBAR_SIZE, thumb_height, DISPLAY_COLORS[DARK_GREY]);
}