#ifndef RECT_HPP
#define RECT_HPP

#include "helpers.hpp"

class Rect : public Control
{
public:
	int	radius;

	Rect() : radius(0) {}

	void	draw(Display *display) override
	{
		control_round_rect(display, computed_location.x, computed_location.y, computed_width, computed_height, radius, bg_color);
		control_children(*this, display);
	}
};
#endif
