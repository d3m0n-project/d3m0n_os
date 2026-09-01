#ifndef APP_CONTROLS_HELPERS_HPP
#define APP_CONTROLS_HELPERS_HPP

#include "app.hpp"

#define ANCHOR_LEFT 1
#define ANCHOR_RIGHT 2
#define ANCHOR_TOP 4
#define ANCHOR_BOTTOM 8
#define ANCHOR_CENTER_X 16
#define ANCHOR_CENTER_Y 32

inline void control_inner(const Control &control, int &x, int &y, int &w, int &h)
{
	x = control.computed_location.x + control.margin_left.get();
	y = control.computed_location.y + control.margin_top.get();
	w = control.computed_width - control.margin_left.get() - control.margin_right.get();
	h = control.computed_height - control.margin_top.get() - control.margin_bottom.get();
	if (w < 0) w = 0;
	if (h < 0) h = 0;
}

inline void control_text_position(int anchor, int x, int y, int w, int h, int tw, int th, int &out_x, int &out_y)
{
	out_x = (anchor & ANCHOR_LEFT) ? x : (anchor & ANCHOR_RIGHT) ? x + w - tw : x + (w - tw) / 2;
	out_y = (anchor & ANCHOR_TOP) ? y : (anchor & ANCHOR_BOTTOM) ? y + h - th : y + (h - th) / 2;
	if (out_x < x) out_x = x;
	if (out_y < y) out_y = y;
}

inline int control_font_size(int requested, int width, int height, const char *text)
{
	if (requested > 0)
		return requested;
	int length = 0;
	if (text)
		while (text[length] && text[length] != '\n') ++length;
	int from_width = length ? width / length : 0;
	int from_height = height / 2;
	int result = from_width && from_height ? (from_width < from_height ? from_width : from_height) : (from_width ? from_width : from_height);
	return result > 0 ? result : 8;
}

inline void control_text(Display *display, const Control &control, const char *text, int align, int requested_size)
{
	int x, y, w, h;
	control_inner(control, x, y, w, h);
	int size = control_font_size(requested_size, w, h, text);
	int lines = 1;
	int longest = 0;
	int current = 0;
	if (text)
		for (int i = 0; text[i]; ++i)
		{
			if (text[i] == '\n')
			{
				if (current > longest)
				{
					longest = current;
					current = 0;
				}
				++lines;
			}
			else
				++current;
		}
	if (current > longest)
		longest = current;
	int tx, ty;
	control_text_position(align, x, y, w, h, size * longest, size * 2 * lines, tx, ty);
	display->draw_text(tx, ty, size, size * 2, text, control.color, 0);
}

inline void control_children(Control &control, Display *display)
{
	Control *child = control.controls;
	while (child)
	{
		if (child->visible)
			child->draw(display);
		child = child->next;
	}
}

inline void control_round_rect(Display *display, int x, int y, int w, int h, int radius, uint32_t color)
{
	if (radius <= 0)
	{
		display->draw_rect(x, y, w, h, color);
		return;
	}
	if (radius > w / 2)
		radius = w / 2;
	if (radius > h / 2)
		radius = h / 2;
	display->draw_rect(x + radius, y, w - radius * 2, h, color);
	display->draw_rect(x, y + radius, w, h - radius * 2, color);
	display->draw_ellipse(x + radius, y + radius, radius, radius, color, 1);
	display->draw_ellipse(x + w - radius - 1, y + radius, radius, radius, color, 1);
	display->draw_ellipse(x + radius, y + h - radius - 1, radius, radius, color, 1);
	display->draw_ellipse(x + w - radius - 1, y + h - radius - 1, radius, radius, color, 1);
}

#endif
