#ifndef ROUNDBUTTON_HPP
#define ROUNDBUTTON_HPP

#include "helpers.hpp"

/** @brief Control that draws a rounded button with text. */
class RoundButton : public Control
{
public:
	string		content;
	string		image;
	int			font_size;
	int			text_align;
	int			radius;

	RoundButton() : content(), image(), font_size(0), text_align(0), radius(0) {}

	/** @brief Draws this control and, where applicable, its child controls. */
	void	draw(Display *display) override
	{
		control_round_rect(display, computed_location.x, computed_location.y, computed_width, computed_height, radius, bg_color);
		control_text(display, *this, content.c_str(), text_align, font_size);
	}
};
#endif
