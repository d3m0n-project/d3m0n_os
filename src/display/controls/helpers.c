#include "controls_graphics.h"

void	compute_text_position(int anchor, int box_x, int box_y, int box_w, int box_h, int text_w, int text_h, int *out_x, int *out_y)
{
	int tx = box_x;
	int ty = box_y;
	if (anchor & ANCHOR_LEFT)
		tx = box_x;
	else if (anchor & ANCHOR_RIGHT)
		tx = box_x + box_w - text_w;
	else
		tx = box_x + (box_w - text_w) / 2;

	if (anchor & ANCHOR_TOP)
		ty = box_y;
	else if (anchor & ANCHOR_BOTTOM)
		ty = box_y + box_h - text_h;
	else
		ty = box_y + (box_h - text_h) / 2;

	if (tx < box_x) tx = box_x;
	if (ty < box_y) ty = box_y;
	*out_x = tx;
	*out_y = ty;
}

void	compute_inner_rect(const t_control *control, int *out_x, int *out_y, int *out_w, int *out_h)
{
	int x = control->location.x + control->margin_left;
	int y = control->location.y + control->margin_top;
	int w = control->width - control->margin_left - control->margin_right;
	int h = control->height - control->margin_top - control->margin_bottom;
	if (w < 0) w = 0;
	if (h < 0) h = 0;
	*out_x = x;
	*out_y = y;
	*out_w = w;
	*out_h = h;
}

void	control_text_metrics(const char *text, int *out_max_len, int *out_lines)
{
	int i = 0;
	int cur = 0;
	int max = 0;
	int lines = 1;

	if (!text)
	{
		if (out_max_len) *out_max_len = 0;
		if (out_lines) *out_lines = 0;
		return;
	}
	while (text[i])
	{
		if (text[i] == '\n')
		{
			if (cur > max) max = cur;
			cur = 0;
			lines++;
		}
		else
			cur++;
		i++;
	}
	if (cur > max) max = cur;
	if (out_max_len) *out_max_len = max;
	if (out_lines) *out_lines = lines;
}

int	control_text_auto_font_size(const t_control *control)
{
	int	len;
	int	from_width;
	int	from_height;
	int	size;
	int	lines;

	if (!control)
		return (8);
	if (control->font_size > 0)
		return (control->font_size);
	len = 0;
	lines = 0;
	control_text_metrics(control->content, &len, &lines);
	from_width = 0;
	from_height = 0;
	if (control->width > 0 && len > 0)
		from_width = control->width / len;
	if (control->height > 0)
		from_height = control->height / 2;
	if (from_width > 0 && from_height > 0)
		size = (from_width < from_height) ? from_width : from_height;
	else if (from_width > 0)
		size = from_width;
	else if (from_height > 0)
		size = from_height;
	else
		size = 8;
	if (size < 1)
		size = 1;
	return (size);
}