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
			// compute child's absolute position
			int saved_x = child->location.x;
			int saved_y = child->location.y;

			child->location.x = control->location.x + saved_x;
			child->location.y = control->location.y + saved_y - control->p_scroll_offset.y;

			// only draw if visible in the vscroll area
			if (child->location.y + child->height >= control->location.y && child->location.y <= control->location.y + control->height)
				draw_control(child);

			// restore
			child->location.x = saved_x;
			child->location.y = saved_y;

			child = child->p_next;
		}
	}
	if (control->bar)
	{
		int scroll_thumb_offset = 0;
		if (control->p_scroll_max_size.y > 0)
			scroll_thumb_offset = (control->height * control->p_scroll_offset.y) / control->p_scroll_max_size.y;
		draw_rect(control->location.x + control->width - SCROLLBAR_SIZE, control->location.y, SCROLLBAR_SIZE, control->height, DISPLAY_COLORS[GREY]);

		// scrollbar thumb
		int thumbHeight = SCROLLBAR_THUMB_MIN_HEIGHT;
		if (control->p_scroll_max_size.y > 0)
			thumbHeight = (control->height * control->height) / (control->p_scroll_max_size.y + control->height);
		thumbHeight = max(thumbHeight, SCROLLBAR_THUMB_MIN_HEIGHT);
		draw_rect(control->location.x + control->width - SCROLLBAR_SIZE, control->location.y + scroll_thumb_offset, SCROLLBAR_SIZE, thumbHeight, DISPLAY_COLORS[DARK_GREY]);
	}
}