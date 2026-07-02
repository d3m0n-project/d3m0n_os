#include "controls_graphics.h"

void	ctrl_draw_rect(t_control	*control)
{
	draw_rect(control->location.x, control->location.y, control->width, control->height, control->bg_color);
}