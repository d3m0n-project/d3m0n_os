#ifndef HSCROLL_HPP
#define HSCROLL_HPP

#include "helpers.hpp"

class Hscroll : public Control
{
public:
	bool		bar;
	int			scroll;
	int			content_width;

	Hscroll() : bar(true), scroll(0), content_width(0) {}

	void	draw(Display *display) override
	{
		display->draw_rect(computed_location.x, computed_location.y, computed_width, computed_height, bg_color);
		control_children(*this, display);
		if (!bar)
			return;
		int size = 8, thumb = content_width > computed_width ? computed_width * computed_width / content_width : computed_width;
		if (thumb < 12)
			thumb = 12;
		int offset = content_width > computed_width ? scroll * (computed_width - thumb) / (content_width - computed_width) : 0;
		display->draw_rect(computed_location.x, computed_location.y + computed_height - size, computed_width, size, bg_color);
		display->draw_rect(computed_location.x + offset, computed_location.y + computed_height - size, thumb, size, color);
	}
};
#endif
