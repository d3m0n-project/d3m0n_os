#ifndef CHECKBOX_HPP
#define CHECKBOX_HPP
#include "helpers.hpp"
class CheckBox : public Control
{
public:
	string		content;
	int			font_size;
	int			text_align;
	bool		checked;
	CheckBox() : content(), font_size(0), text_align(0), checked(false) {}

	void draw(Display *display) override
	{
		int s = computed_height < computed_width ? computed_height : computed_width;
		if (s < 4)
			s = 4;
		display->draw_rect(computed_location.x, computed_location.y, s, s, checked ? color : bg_color);
		if (checked)
			display->draw_text(computed_location.x + s / 4, computed_location.y, s / 2, s, "X", color, 0);
		if (content.length())
			display->draw_text(computed_location.x + s + 8, computed_location.y, font_size, s, content.c_str(), color, 0);
	}
};
#endif
