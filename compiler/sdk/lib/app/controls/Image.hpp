#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "helpers.hpp"
#include "stdlib.h"
#include "app/parsing/svg.h"

/** @brief Control that loads and draws an image from its source path. */
class Image : public Control
{
public:
	string	source;
	int		mode;

	Image() : source(), mode(0) {}

	/** @brief Draws this control and, where applicable, its child controls. */
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

		display->draw_rect(this->computed_location.x, this->computed_location.y, this->computed_width, this->computed_height, this->bg_color);
		if (strcmp((char *)extension, (char *)"svg") == 0 || 1)
		{
			int ret = display->draw_svg(this->computed_location.x, this->computed_location.y, this->computed_width, this->computed_height, this->source.c_str());
			if (ret == 1)
				printf("Image: svg at '%s' does not exists!\n", this->source.c_str());
			return;
		}
		else
			printf("Unsupported media type in Image src: '%s'\n", extension);
	}
};
#endif
