#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "helpers.hpp"
#include "stdlib.h"
#include "app/svg.h"

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
		if (strcmp((char *)extension, (char *)"svg") == 0 || 1)
		{
			int ret = display->draw_svg(computed_location.x, computed_location.y, computed_width, computed_height, source.c_str());
			if (ret == 1)
				printf("Image: svg at '%s' does not exists!\n", source.c_str());
			printf("DEBUG MSG: drawn image\n");
			return;
		}
		display->draw_rect(computed_location.x, computed_location.y, computed_width, computed_height, bg_color);
	}
};
#endif
