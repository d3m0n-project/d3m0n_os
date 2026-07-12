#include "controls_graphics.h"

void	ctrl_draw_progressbar(t_control *control)
{
	// background
	if (control->bg_color)
		draw_rect(control->p_client_location.x, control->p_client_location.y, control->width, control->height, control->bg_color);

	// draw bar
	int two_padding = PROGRESSBAR_PADDING * 2;
	int x0 = control->p_client_location.x + two_padding;
	int w = control->width - two_padding - two_padding;
	int y0 = control->p_client_location.y + PROGRESSBAR_PADDING;
	int r = PROGRESSBAR_HEIGHT / 2;
	int h = r * 2;

	draw_rect(x0, y0, w, h + 1, PROGRESSBAR_OFF_COLOR);

	// round borders
	draw_ellipse(x0, y0 + r, r, r, PROGRESSBAR_OFF_COLOR, 1);
	draw_ellipse(x0 + w, y0 + r, r, r, PROGRESSBAR_OFF_COLOR, 1);

	// thumb
	float thumb_percentage = 0.0f;
	if (control->max != control->min)
	{
		thumb_percentage = (float)(control->value - control->min) / (float)(control->max - control->min);

		// clamp
		if (thumb_percentage < 0.0f)
			thumb_percentage = 0.0f;
		else if (thumb_percentage > 1.0f)
			thumb_percentage = 1.0f;
	}

	int thumb_x = x0 + (int)(w * thumb_percentage);
	draw_ellipse(thumb_x, y0 + r, h, h, control->color, 1);
}