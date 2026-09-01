#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "helpers.hpp"

class Image : public Control
{
public:
	string	source;
	int		mode;

	Image() : source(), mode(0) {}

	void	draw(Display *display) override
	{
		const char *path = source.c_str();
		const char *extension = path;
		while (*path)
		{
			if (*path == '.')
				extension = path + 1;
			++path;
		}
		if (extension[0] == 's' && extension[1] == 'v' && extension[2] == 'g' && extension[3] == '\0')
		{
			display->draw_svg(computed_location.x, computed_location.y, computed_width, computed_height, source.c_str());
			return;
		}
		display->draw_rect(computed_location.x, computed_location.y, computed_width, computed_height, bg_color);
	}
};
#endif
