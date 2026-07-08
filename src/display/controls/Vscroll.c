#include "controls_graphics.h"
#include "math.h"
#include "controls.h"

void	ctrl_draw_vscroll(t_control	*control)
{
	draw_rect(control->location.x, control->location.y, control->width, control->height, control->bg_color);
    
	// sraw children offset by scroll
	if (control->children)
	{
		t_control *child = control->children;
		while (child)
		{
			if (child == control) // avoid infinite loop
				break;
			// compute child's absolute position
			int saved_x = child->location.x;
			int saved_y = child->location.y;

			child->p_client_location = (t_point){
				.x=control->p_client_location.x + saved_x,
				.y=control->p_client_location.y + saved_y - control->p_scroll_offset.y
			};

			// only draw if visible in the vscroll area
			if (child->p_client_location.y + child->height >= control->p_client_location.y && child->p_client_location.y <= control->p_client_location.y + control->height)
				draw_control(child);

			child = child->p_next;
		}
	}
	if (control->bar)
	{
		int scroll_thumb_offset = 0;
		if (control->p_scroll_max_size.y > 0)
			scroll_thumb_offset = (control->height * control->p_scroll_offset.y) / control->p_scroll_max_size.y;
		//scroll_thumb_offset /= 2;
		draw_rect(control->p_client_location.x + control->width - SCROLLBAR_SIZE, control->p_client_location.y, SCROLLBAR_SIZE, control->height, DISPLAY_COLORS[GREY]);

		// scrollbar thumb
		int thumbHeight = SCROLLBAR_THUMB_MIN_HEIGHT;
		if (control->p_scroll_max_size.y > 0)
			thumbHeight = (control->height * control->height) / (control->p_scroll_max_size.y + control->height);
		thumbHeight = max(thumbHeight / 2, SCROLLBAR_THUMB_MIN_HEIGHT);
		draw_rect(control->p_client_location.x + control->width - SCROLLBAR_SIZE, control->p_client_location.y + scroll_thumb_offset, SCROLLBAR_SIZE, thumbHeight, DISPLAY_COLORS[DARK_GREY]);
		// TODO: fix scrollbar height
	}
}