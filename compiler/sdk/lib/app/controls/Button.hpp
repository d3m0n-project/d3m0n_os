#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "helpers.hpp"

class Button : public Control
{
public:
	string		content;
	string		image;
	int			font_size;
	int			text_align;

	Button() : content(), image(), font_size(0), text_align(0) {}

	void draw(Display *display) override
	{
		int x, y, w, h;
		control_inner(*this, x, y, w, h);
		if ((uint32_t)bg_color)
			display->draw_rect(x, y, w, h, bg_color);
		control_text(display, *this, content.c_str(), text_align, font_size);
	}
};
#endif
