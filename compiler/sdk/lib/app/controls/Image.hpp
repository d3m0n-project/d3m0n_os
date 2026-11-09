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
	uint32_t		*icon_pixels = 0;
	uint32_t		icon_width = 0;
	uint32_t		icon_height = 0;
public:
	string			source;
	int				mode;

	Image() : source(), mode(0) {}

	/** @brief Draws this control and, where applicable, its child controls. */
	void	draw(Display *display) override
	{
		if (icon_pixels)
		{
			for (uint32_t y = 0; y < icon_height; ++y)
			{
				int top = computed_location.y + (int)((uint64_t)y * computed_height / icon_height);
				int bottom = computed_location.y + (int)((uint64_t)(y + 1) * computed_height / icon_height);
				for (uint32_t x = 0; x < icon_width; ++x)
				{
					uint32_t color = icon_pixels[y * icon_width + x];
					if ((color >> 24) == 0)
						continue;
					int left = computed_location.x + (int)((uint64_t)x * computed_width / icon_width);
					int right = computed_location.x + (int)((uint64_t)(x + 1) * computed_width / icon_width);
					display->draw_rect(left, top, right - left, bottom - top, color);
				}
			}
			return;
		}
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
			this->image_buf = image_read_to_buf(this->source.c_str(), &this->image_buf_size);

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

	int	load_image_from_buf(char *buf, size_t buf_size)
	{
		if (!buf || buf_size < 22 || (uint8_t)buf[0] || (uint8_t)buf[1] || (uint8_t)buf[2] != 1)
			return 1;
		
		uint16_t count = *(uint16_t *)(buf + 4);
		if (!count || buf_size < 6 + (size_t)count * 16)
			return 1;
		
		uint32_t offset = *(uint32_t *)(buf + 18);
		uint32_t length = *(uint32_t *)(buf + 14);
		if (offset > buf_size || length > buf_size - offset || length < 40)
			return 1;
		
		uint8_t *dib = (uint8_t *)buf + offset;
		uint32_t header_size = *(uint32_t *)dib;
		int32_t width = *(int32_t *)(dib + 4);
		int32_t doubled_height = *(int32_t *)(dib + 8);
		uint16_t planes = *(uint16_t *)(dib + 12);
		uint16_t bits = *(uint16_t *)(dib + 14);
		uint32_t compression = *(uint32_t *)(dib + 16);
		if (header_size != 40 || width <= 0 || doubled_height <= 0 || planes != 1 || bits != 32 || compression != 0)
			return 1;
		
		uint32_t height = (uint32_t)doubled_height / 2;
		if (!height || (uint64_t)width * height * 4 > length - header_size)
			return 1;
		
		uint32_t *pixels = (uint32_t *)malloc((size_t)width * height * sizeof(uint32_t));
		if (!pixels)
			return 1;
		
		const uint8_t *src = dib + header_size;
		for (int32_t y = 0; y < (int32_t)height; ++y)
		{
			for (int32_t x = 0; x < width; ++x)
			{
				const uint8_t *pixel = src + ((size_t)(height - 1 - y) * width + x) * 4;
				pixels[(size_t)y * width + x] = ((uint32_t)pixel[3] << 24) | ((uint32_t)pixel[0] << 16) | ((uint32_t)pixel[1] << 8) | pixel[2];
			}
		}

		if (icon_pixels)
			free(icon_pixels);
		
		icon_pixels = pixels;
		icon_width = width;
		icon_height = height;
		return 0;
	}

	~Image()
	{
		if (this->png_cache.pixels)
			free(this->png_cache.pixels);
		if (icon_pixels)
			free(icon_pixels);
		if (this->image_buf)
			free(this->image_buf);
	}
};
#endif
