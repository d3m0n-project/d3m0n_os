#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "helpers.hpp"

class TextBox : public Control
{
public:
	string		content;
	string		type;
	int			font_size;
	int			text_align;

	TextBox() : content(), type(), font_size(0), text_align(0) {}

	void	draw(Display *display) override
	{
		int x, y, w, h; control_inner(*this, x, y, w, h);
		display->draw_rect(x, y, w, h, bg_color);
		if (type.length() && type[0] == 'p')
		{
			char hidden[128];
			size_t length = content.length() < 127 ? content.length() : 127;
			for (size_t i = 0; i < length; ++i)
				hidden[i] = '*';
			hidden[length] = '\0';
			control_text(display, *this, hidden, text_align, font_size);
		}
		else
			control_text(display, *this, content.c_str(), text_align, font_size);
	}
};
#endif
