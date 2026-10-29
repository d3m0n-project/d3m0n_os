#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "helpers.hpp"
#include "stdlib.h"
#include "app/parsing/image_read.h"
#include "app/parsing/svg.h"

/** @brief Control that loads and draws an image from its source path. */
class Image : public Control
{
private:
	unsigned char	*image_buf = 0;
	size_t			image_buf_size = 0;

	t_png_cache		png_cache = {.image_width=0, .image_height=0, .pixels=0};
public:
	string			source;
	int				mode;

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
		
		// load image into heap
		if (!this->image_buf)
		{
			printf("loading image: %s...\n", this->source.c_str());
			this->image_buf = image_read_to_buf(this->source.c_str(), &this->image_buf_size);
		}

		if (!this->image_buf)
		{
			fprintf(stderr, "Image: Could not find or read '%s' as image source!\n", this->source.c_str());
			return;
		}
		
		if (strcmp((char *)extension, (char *)"svg") == 0)
		{
			display->draw_svg_buff(this->computed_location.x, this->computed_location.y, this->computed_width, this->computed_height, (char *)this->image_buf, this->image_buf_size);
			return;
		}
		else if (strcmp((char *)extension, (char *)"png") == 0)
		{
			int ret = display->draw_png(this->computed_location.x, this->computed_location.y, this->computed_width, this->computed_height, (char *)this->image_buf, this->image_buf_size, &this->png_cache);
			if (ret == 1)
				printf("Image: png at '%s' could not be drawn!\n", this->source.c_str());
			return;
		}
		else
			printf("Unsupported media type in Image src: '%s'\n", extension);
	}

	~Image()
	{
		if (this->png_cache.pixels)
			free(this->png_cache.pixels);
		if (this->image_buf)
			free(this->image_buf);
	}
};
#endif
