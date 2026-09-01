#ifndef TEXT_HPP
#define TEXT_HPP

#include "helpers.hpp"

class Text : public Control
{
public:
	string	content;
	int		font_size;
	int		text_align;

	Text() : content(), font_size(0), text_align(0) {}

	void	draw(Display *display) override
	{
		int x, y, w, h;
		control_inner(*this, x, y, w, h);
		if ((uint32_t)bg_color)
			display->draw_rect(x, y, w, h, bg_color);
		control_text(display, *this, content.c_str(), text_align, font_size);
	}
};
#endif
