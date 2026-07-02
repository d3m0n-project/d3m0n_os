#include "controls_graphics.h"

void	ctrl_draw_text(t_control *control)
{
	int inner_x, inner_y, inner_w, inner_h;
	int char_w, char_h, text_w, text_h;
	int draw_x, draw_y;
	int maxlen = 0;
	int lines = 0;
	int font_size = control_text_auto_font_size(control);
	char_w = font_size;
	char_h = font_size * 2;
	compute_inner_rect(control, &inner_x, &inner_y, &inner_w, &inner_h);
	control_text_metrics(control->content, &maxlen, &lines);
	text_w = char_w * maxlen;
	text_h = char_h * (lines > 0 ? lines : 1);
	if (control->bg_color)
		draw_rect(inner_x, inner_y, inner_w, inner_h, control->bg_color);
	compute_text_position(control->text_align, inner_x, inner_y, inner_w, inner_h, text_w, text_h, &draw_x, &draw_y);
	draw_text(draw_x, draw_y, char_w, char_h, control->content, control->color, 0);
}