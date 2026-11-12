#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "helpers.hpp"
#include "app/parsing/image_read.h"
#include "app/parsing/svg.h"
#include "parsing/ico.h"

/** @brief Control that loads and draws an image from its source path. */
class Image : public Control
{
private:
	unsigned char	*image_buf = 0;
	size_t			image_buf_size = 0;
	bool			inline_svg = false;

	t_png_cache		png_cache = {.image_width=0, .image_height=0, .pixels=0};
	uint32_t		*icon_pixels = 0;
	uint32_t		icon_width = 0;
	uint32_t		icon_height = 0;

public:
	string			source;
	int				mode;

	Image() : source(), mode(0)
	{
		this->color = Color(0, 0, 0, 0);
	}

	/** @brief Draws this control and, where applicable, its child controls. */
	void	draw(Display *display) override
	{
		if (inline_svg && image_buf)
		{
			uint32_t color = ((this->color.value & 0xff000000) > 0)?this->color.value:0x00000000;
			display->draw_svg_buff(computed_location.x, computed_location.y, computed_width, computed_height, (const char *)image_buf, image_buf_size, color);
			return;
		}
		if (icon_pixels)
		{
			if (computed_width <= 0 || computed_height <= 0 || !icon_width || !icon_height)
				return;
			for (int y = 0; y < computed_height; ++y)
			{
				float source_y = ((y + 0.5f) * icon_height / computed_height) - 0.5f;
				if (source_y < 0) source_y = 0;
				if (source_y > icon_height - 1) source_y = (float)icon_height - 1;
				uint32_t y0 = (uint32_t)source_y;
				uint32_t y1 = y0 + 1 < icon_height ? y0 + 1 : y0;
				float fy = source_y - y0;
				for (int x = 0; x < computed_width; ++x)
				{
					float source_x = ((x + 0.5f) * icon_width / computed_width) - 0.5f;
					if (source_x < 0)
						source_x = 0;
					
					if (source_x > icon_width - 1)
						source_x = (float)icon_width - 1;
					
					uint32_t x0 = (uint32_t)source_x;
					uint32_t x1 = x0 + 1 < icon_width ? x0 + 1 : x0;
					float fx = source_x - x0;
					float weights[4] = {(1.0f - fx) * (1.0f - fy), fx * (1.0f - fy), (1.0f - fx) * fy, fx * fy};
					uint32_t samples[4] = {
						icon_pixels[(size_t)y0 * icon_width + x0],
						icon_pixels[(size_t)y0 * icon_width + x1],
						icon_pixels[(size_t)y1 * icon_width + x0],
						icon_pixels[(size_t)y1 * icon_width + x1]
					};
					float alpha = 0, red = 0, green = 0, blue = 0;
					for (int i = 0; i < 4; ++i)
					{
						float sample_alpha = (samples[i] >> 24) & 0xff;
						float weight = weights[i];
						alpha += sample_alpha * weight;
						red += (samples[i] & 0xff) * sample_alpha * weight;
						green += ((samples[i] >> 8) & 0xff) * sample_alpha * weight;
						blue += ((samples[i] >> 16) & 0xff) * sample_alpha * weight;
					}
					if (alpha <= 0.0f)
						continue;
					uint32_t background = display->get_pixel(computed_location.x + x, computed_location.y + y);
					float inverse_alpha = 255.0f - alpha;
					uint32_t out_red = (uint32_t)((red + (background & 0xff) * inverse_alpha + 127.0f) / 255.0f);
					uint32_t out_green = (uint32_t)((green + ((background >> 8) & 0xff) * inverse_alpha + 127.0f) / 255.0f);
					uint32_t out_blue = (uint32_t)((blue + ((background >> 16) & 0xff) * inverse_alpha + 127.0f) / 255.0f);
					display->put_pixel(computed_location.x + x, computed_location.y + y, 0xff000000 | (out_blue << 16) | (out_green << 8) | out_red);
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
			uint32_t color = ((this->color.value & 0xff000000) > 0)?this->color.value:0x00000000;
			display->draw_svg_buff(this->computed_location.x, this->computed_location.y, this->computed_width, this->computed_height, (char *)this->image_buf, this->image_buf_size, color);
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
		if (!buf || buf_size < 6 || ico_parser_u16((unsigned char *)buf) != 0 || ico_parser_u16((unsigned char *)buf + 2) != 1)
			return 1;
		uint16_t count = ico_parser_u16((unsigned char *)buf + 4);
		if (!count || (size_t)count > (buf_size - 6) / 16)
			return 1;
		uint32_t *pixels = 0, width = 0, height = 0;
		uint64_t best_area = 0;
		for (uint16_t i = 0; i < count; ++i)
		{
			const unsigned char *entry = (unsigned char *)buf + 6 + (size_t)i * 16;
			uint32_t offset = ico_parser_u32(entry + 12);
			uint32_t length = ico_parser_u32(entry + 8);
			if (!length || offset > buf_size || length > buf_size - offset)
				continue;
			uint32_t decoded_width = 0, decoded_height = 0, *decoded_pixels = 0;
			if (!ico_decode_image((unsigned char *)buf + offset, length, &decoded_width,
				&decoded_height, &decoded_pixels))
				continue;
			uint64_t area = (uint64_t)decoded_width * decoded_height;
			if (area > best_area)
			{
				if (pixels) free(pixels);
				pixels = decoded_pixels;
				width = decoded_width;
				height = decoded_height;
				best_area = area;
			}
			else
				free(decoded_pixels);
		}
		if (!pixels) return 1;

		if (icon_pixels)
			free(icon_pixels);
		
		icon_pixels = pixels;
		icon_width = width;
		icon_height = height;
		return 0;
	}

	int	load_svg_from_buf(const char *buf, size_t buf_size)
	{
		if (!buf || !buf_size || buf_size == (size_t)-1)
			return 1;
		unsigned char *copy = (unsigned char *)malloc(buf_size + 1);
		if (!copy)
			return 1;
		memcpy(copy, buf, buf_size);
		copy[buf_size] = 0;
		if (image_buf)
			free(image_buf);
		image_buf = copy;
		image_buf_size = buf_size;
		inline_svg = true;
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
