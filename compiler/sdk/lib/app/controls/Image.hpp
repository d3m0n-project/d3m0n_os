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

	static uint16_t ico_u16(const unsigned char *p)
	{
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}

	static uint32_t ico_u32(const unsigned char *p)
	{
		return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
	}

	static uint32_t ico_channel(uint32_t pixel, uint32_t mask)
	{
		if (!mask) return 0;
		unsigned shift = 0;
		while (((mask >> shift) & 1u) == 0u) ++shift;
		uint32_t max_value = mask >> shift;
		return (((pixel & mask) >> shift) * 255u + max_value / 2u) / max_value;
	}

	static bool decode_ico_image(const unsigned char *data, size_t size,
		uint32_t *width, uint32_t *height, uint32_t **pixels)
	{
		static const unsigned char png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
		if (size >= 8 && png_bytes_equal(data, png_signature, 8))
			return png_decode(data, size, width, height, pixels) != 0;
		
		if (size < 40)
			return false;

		uint32_t header_size = ico_u32(data);
		int32_t dib_width = (int32_t)ico_u32(data + 4);
		int32_t doubled_height = (int32_t)ico_u32(data + 8);
		uint16_t bits = ico_u16(data + 14);
		uint32_t compression = ico_u32(data + 16);
		uint32_t colors_used = ico_u32(data + 32);
		if (header_size < 40 || header_size > size || dib_width <= 0 || doubled_height <= 0 ||
			(doubled_height & 1) || ico_u16(data + 12) != 1 ||
			(bits != 1 && bits != 4 && bits != 8 && bits != 16 && bits != 24 && bits != 32) ||
			(compression != 0 && compression != 3 && compression != 6) ||
			(compression && bits != 16 && bits != 32))
			return false;

		uint32_t w = (uint32_t)dib_width;
		uint32_t h = (uint32_t)doubled_height / 2;
		if ((uint64_t)w * h > (uint64_t)size * 8)
			return false;
		
		size_t xor_stride = (((size_t)w * bits + 31) / 32) * 4;
		size_t xor_bytes = xor_stride * h;
		if (h && xor_bytes / h != xor_stride)
			return false;

		uint32_t masks[4] = {0, 0, 0, 0};
		size_t metadata_size = header_size;
		if (compression == 3 || compression == 6)
		{
			if (header_size >= 52)
			{
				masks[0] = ico_u32(data + 40);
				masks[1] = ico_u32(data + 44);
				masks[2] = ico_u32(data + 48);
				if (header_size >= 56)
					masks[3] = ico_u32(data + 52);
			}
			else
			{
				unsigned mask_count = compression == 6 ? 4 : 3;
				if ((size_t)mask_count * 4 > size - metadata_size)
					return false;
				for (unsigned i = 0; i < mask_count; ++i)
					masks[i] = ico_u32(data + metadata_size + i * 4);
				metadata_size += mask_count * 4;
			}
		}
		else if (bits == 16)
		{
			masks[0] = 0x7c00;
			masks[1] = 0x03e0;
			masks[2] = 0x001f;
		}
		else if (bits == 32)
		{
			masks[0] = 0x00ff0000;
			masks[1] = 0x0000ff00;
			masks[2] = 0x000000ff;
			masks[3] = 0xff000000;
		}
		uint32_t palette_count = 0;
		if (bits <= 8)
		{
			palette_count = colors_used ? colors_used : (1u << bits);
			if (palette_count > (1u << bits)) return false;
		}

		size_t palette_bytes = (size_t)palette_count * 4;
		if (palette_bytes > size - metadata_size)
			return false;
		
		size_t pixel_offset = metadata_size + palette_bytes;
		if (xor_bytes > size - pixel_offset)
			return false;
		
		size_t and_stride = (((size_t)w + 31) / 32) * 4;
		size_t and_bytes = and_stride * h;
		
		bool has_and = and_bytes <= size - pixel_offset - xor_bytes;
		const unsigned char *palette = data + metadata_size;
		const unsigned char *xor_bitmap = data + pixel_offset;
		const unsigned char *and_bitmap = has_and ? xor_bitmap + xor_bytes : 0;
		uint32_t *output = (uint32_t *)malloc((size_t)w * h * sizeof(uint32_t));
		if (!output)
			return false;

		bool has_alpha = false;
		if (bits == 32 && compression == 0)
			for (uint32_t y = 0; y < h && !has_alpha; ++y)
				for (uint32_t x = 0; x < w; ++x)
					if (xor_bitmap[(size_t)y * xor_stride + (size_t)x * 4 + 3])
					{
						has_alpha = true;
						break;
					}
		if (compression && masks[3])
			has_alpha = true;
		for (uint32_t y = 0; y < h; ++y)
		{
			const unsigned char *row = xor_bitmap + (size_t)(h - 1 - y) * xor_stride;
			const unsigned char *mask = has_and ? and_bitmap + (size_t)(h - 1 - y) * and_stride : 0;
			for (uint32_t x = 0; x < w; ++x)
			{
				uint32_t r = 0, g = 0, b = 0, a = 255;
				if (bits <= 8)
				{
					uint32_t index = bits == 8 ? row[x] : bits == 4 ?
						((row[x / 2] >> ((x & 1) ? 0 : 4)) & 0xf) :
						((row[x / 8] >> (7 - (x & 7))) & 1);
					if (index >= palette_count)
					{
						free(output);
						return false;
					}
					const unsigned char *entry = palette + index * 4;
					b = entry[0]; g = entry[1]; r = entry[2];
				}
				else if (bits == 24)
				{
					const unsigned char *pixel = row + (size_t)x * 3;
					b = pixel[0]; g = pixel[1]; r = pixel[2];
				}
				else
				{
					uint32_t value = bits == 16 ? ico_u16(row + (size_t)x * 2) : ico_u32(row + (size_t)x * 4);
					r = ico_channel(value, masks[0]);
					g = ico_channel(value, masks[1]);
					b = ico_channel(value, masks[2]);
					if (has_alpha)
						a = ico_channel(value, masks[3]);
				}
				if (mask && ((mask[x / 8] >> (7 - (x & 7))) & 1))
					a = 0;
				output[(size_t)y * w + x] = (a << 24) | (r << 16) | (g << 8) | b;
			}
		}
		*width = w; *height = h; *pixels = output;
		return true;
	}
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
		if (!buf || buf_size < 6 || ico_u16((unsigned char *)buf) != 0 || ico_u16((unsigned char *)buf + 2) != 1)
			return 1;
		uint16_t count = ico_u16((unsigned char *)buf + 4);
		if (!count || (size_t)count > (buf_size - 6) / 16)
			return 1;
		uint32_t *pixels = 0, width = 0, height = 0;
		uint64_t best_area = 0;
		for (uint16_t i = 0; i < count; ++i)
		{
			const unsigned char *entry = (unsigned char *)buf + 6 + (size_t)i * 16;
			uint32_t offset = ico_u32(entry + 12);
			uint32_t length = ico_u32(entry + 8);
			if (!length || offset > buf_size || length > buf_size - offset)
				continue;
			uint32_t decoded_width = 0, decoded_height = 0, *decoded_pixels = 0;
			if (!decode_ico_image((unsigned char *)buf + offset, length, &decoded_width,
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
