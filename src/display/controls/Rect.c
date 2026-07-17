#include "controls_graphics.h"

void ctrl_draw_rect(t_control *control)
{
	int x = control->p_client_location.x;
	int y = control->p_client_location.y;
	int w = control->p_client_size.x;
	int h = control->p_client_size.y;
	int r = control->radius;

	if (r > 0)
	{
		int max_r = min(w, h) / 2;

		if (r > max_r)
			r = max_r;

		// circle
		if (r == w / 2 && r == h / 2)
			draw_ellipse(x + w / 2, y + h / 2, w / 2, h / 2, control->bg_color, 1);
		else
		{
			// middle
			draw_rect(x + r, y, w - 2 * r, h, control->bg_color);
			draw_rect(x, y + r, w, h - 2 * r, control->bg_color);

			// corners
			draw_ellipse(x + r,     y + r,     r, r, control->bg_color, 1); // TL
			draw_ellipse(x + w - r, y + r,     r, r, control->bg_color, 1); // TR
			draw_ellipse(x + r,     y + h - r, r, r, control->bg_color, 1); // BL
			draw_ellipse(x + w - r, y + h - r, r, r, control->bg_color, 1); // BR
		}
	}
	else
		draw_rect(x, y, w, h, control->bg_color);

	if (control->children)
	{
		t_control *child = control->children;

		while (child)
		{
			if (child == control)
				break;

			child->p_scroll_offset = control->p_scroll_offset;
			compute_control_layout(child, control, (t_point){0, -child->p_scroll_offset.y});
			// TODO: check why Rect not relative & remove relative parsing
			//if (child->p_client_location.y + child->p_client_size.y >= y && child->p_client_location.y <= y + h)
				draw_control(child);

			child = child->p_next;
		}
	}
}