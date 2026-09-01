#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "helpers.hpp"

class Switch : public Control
{
public:
	string		content;
	int			font_size;
	int			text_align;
	bool		checked;

	Switch() : content(), font_size(0), text_align(0), checked(false) {}

	void	draw(Display *display) override
	{
		int x = computed_location.x, y = computed_location.y, h = computed_height;
		int w = computed_width < h * 2 ? h * 2 : computed_width, r = h / 2;
		uint32_t track = checked ? (uint32_t)color : (uint32_t)bg_color;
		display->draw_ellipse(x + r, y + r, r, r, track, 1);
		display->draw_ellipse(x + w - r - 1, y + r, r, r, track, 1);
		display->draw_rect(x + r, y, w - r * 2, h, track);
		display->draw_ellipse(checked ? x + w - r - 1 : x + r, y + r, r > 2 ? r - 2 : 1, r > 2 ? r - 2 : 1, 0xFFFFFFFF, 1);
		if (content.length())
			display->draw_text(x + w + 8, y, font_size, h, content.c_str(), color, 0);
	}
};
#endif
