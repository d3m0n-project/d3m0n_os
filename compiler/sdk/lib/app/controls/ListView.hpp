#ifndef LISTVIEW_HPP
#define LISTVIEW_HPP

#include "helpers.hpp"

/** @brief Control that displays a scrollable list of items. */
class ListView : public Control
{
public:
	ListView() {}
	/** @brief Draws this control and, where applicable, its child controls. */
	void draw(Display *display) override
	{
		if ((uint32_t)bg_color)
			display->draw_rect(computed_location.x, computed_location.y, computed_width, computed_height, bg_color);
		control_children(*this, display);
	}
};
#endif
