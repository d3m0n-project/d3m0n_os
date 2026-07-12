#include "controls_graphics.h"

void	ctrl_draw_switch(t_control	*control)
{
	int			x;
	int			y;
	int			w;
	int			h;
	int			r;
	int			thumb_x;
	uint32_t	track_color;
	uint32_t	thumb_color;

	x = control->p_client_location.x;
	y = control->p_client_location.y;
	w = control->width;
	h = control->height;

	if (w < h * 2)
		w = h * 2;

	r = h / 2;


	if (control->checked)
		track_color = control->color;
	else
		track_color = control->bg_color;


	draw_ellipse(x + r, y + r, r, r, track_color, 1);
	draw_ellipse(x + w - r, y + r, r, r, track_color, 1);

	for (int py = y; py <= y + h; py++)
	{
		for (int px = x + r; px < x + w - r; px++)
			put_pixel(px, py, track_color);
	}


	if (control->checked)
		thumb_x = x + w - r;
	else
		thumb_x = x + r;
	thumb_color = 0xFFFFFFFF;
	draw_ellipse(thumb_x, y + r, r - 2, r - 2, thumb_color, 1);


	if (control->content[0])
		draw_text(x + w + 8, y, control->width, h, control->content, control->color, 0);
}