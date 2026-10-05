#ifndef RECT_HPP
#define RECT_HPP

#include "helpers.hpp"

/** @brief Represents the Rect type. */
class Rect : public Control
{
public:
	int	radius;

	Rect() : radius(0) {}

	/** @brief draw operation. */
	void	draw(Display *display) override
	{
		control_round_rect(display, computed_location.x, computed_location.y, computed_width, computed_height, radius, bg_color);
		control_children(*this, display);
	}
};
#endif
