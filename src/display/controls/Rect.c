#include "controls_graphics.h"

void	ctrl_draw_rect(t_control	*control)
{
	//t_window *current_win = get_current_window();
	int y = control->p_client_location.y;
	int h = control->height;
	//if (current_win->top_bar && y <= TOPBAR_HEIGHT)
	//{
	//	log("y=%i   delta = %i\n", 0, y, ABS(TOPBAR_HEIGHT - y));
	//	y = TOPBAR_HEIGHT;
	//	h -= ABS(TOPBAR_HEIGHT - y);
	//}

	// TODO: check that no control overrides on the topbar

	draw_rect(control->p_client_location.x, y, control->width, h, control->bg_color);
}