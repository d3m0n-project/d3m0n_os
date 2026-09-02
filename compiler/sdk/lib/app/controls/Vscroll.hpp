#ifndef VSCROLL_HPP
#define VSCROLL_HPP

#include "helpers.hpp"

class Vscroll : public Control
{
public:
	bool	bar;
	int		scroll;
	int		content_height;

	Vscroll() : bar(true), scroll(0), content_height(0) {}

	void	draw(Display *display) override
	{
		display->draw_rect(computed_location.x, computed_location.y, computed_width, computed_height, bg_color);
		control_children(*this, display);
		if (!bar)
			return;
		int size = 8, thumb = content_height > computed_height ? computed_height * computed_height / content_height : computed_height;
		if (thumb < 12)
			thumb = 12;
		int offset = content_height > computed_height ? scroll * (computed_height - thumb) / (content_height - computed_height) : 0;
		display->draw_rect(computed_location.x + computed_width - size, computed_location.y, size, computed_height, bg_color);
		display->draw_rect(computed_location.x + computed_width - size, computed_location.y + offset, size, thumb, color);
	}
};
#endif
