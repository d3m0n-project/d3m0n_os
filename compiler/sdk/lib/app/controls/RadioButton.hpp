#ifndef RADIOBUTTON_HPP
#define RADIOBUTTON_HPP

#include "helpers.hpp"

class RadioButton : public Control
{
public:
	string		content;
	int			font_size;
	int			text_align;
	bool		checked;

	RadioButton() : content(), font_size(0), text_align(0), checked(false) {}

	void draw(Display *display) override
	{
		int r = computed_height < computed_width ? computed_height / 2 : computed_width / 2;
		if (r < 2)
			r = 2;
		display->draw_ellipse(computed_location.x + r, computed_location.y + r, r, r, checked ? color : bg_color, 1);
		if (checked && r > 3)
			display->draw_ellipse(computed_location.x + r, computed_location.y + r, r / 2, r / 2, color, 1);
		if (content.length())
			display->draw_text(computed_location.x + r * 2 + 8, computed_location.y, font_size, r * 2, content.c_str(), color, 0);
	}
};
#endif
