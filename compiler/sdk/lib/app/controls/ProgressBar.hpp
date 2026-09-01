#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include "helpers.hpp"

class ProgressBar : public Control
{
public:
	int					min;
	int					value;
	int					max;
	static const int	padding = 4;
	static const int	bar_height = 8;

	ProgressBar()
	{
		this->min = 0;
		this->value = 0;
		this->max = 100;
	}

	void draw(Display *display) override
	{
		int x = computed_location.x + padding;
		int y = computed_location.y + padding;
		int w = computed_width - padding * 2;
		if (w < 1)
			w = 1;
		int r = bar_height / 2;
		display->draw_rect(x + r, y, w - r * 2, bar_height, bg_color);
		display->draw_ellipse(x + r, y + r, r, r, bg_color, 1);
		display->draw_ellipse(x + w - r - 1, y + r, r, r, bg_color, 1);
		int range = max - min, amount = range ? (value - min) * w / range : 0;
		if (amount < 0)
			amount = 0;
		if (amount > w)
			amount = w;
		display->draw_rect(x, y, amount, bar_height, color);
	}
};
#endif
