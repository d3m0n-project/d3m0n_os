#ifndef SDK_ICO_PARSER_H
#define SDK_ICO_PARSER_H

#include "app/parsing/png.h"

/* Decode an ICO entry into the SDK framebuffer format: 0xAABBGGRR. */
	static inline uint16_t ico_parser_u16(const unsigned char *p)
	{
		return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	}

	static inline uint32_t ico_parser_u32(const unsigned char *p)
	{
		return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
	}

	static inline uint32_t ico_parser_channel(uint32_t pixel, uint32_t mask)
	{
		if (!mask) return 0;
		unsigned shift = 0;
		while (((mask >> shift) & 1u) == 0u) ++shift;
		uint32_t max_value = mask >> shift;
		return (((pixel & mask) >> shift) * 255u + max_value / 2u) / max_value;
	}

	static inline bool ico_decode_image(const unsigned char *data, size_t size,
		uint32_t *width, uint32_t *height, uint32_t **pixels)
	{
		static const unsigned char png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
		if (size >= 8 && png_bytes_equal(data, png_signature, 8))
			return png_decode(data, size, width, height, pixels) != 0;
		
		if (size < 40)
			return false;

		uint32_t header_size = ico_parser_u32(data);
		int32_t dib_width = (int32_t)ico_parser_u32(data + 4);
		int32_t doubled_height = (int32_t)ico_parser_u32(data + 8);
		uint16_t bits = ico_parser_u16(data + 14);
		uint32_t compression = ico_parser_u32(data + 16);
		uint32_t colors_used = ico_parser_u32(data + 32);
		if (header_size < 40 || header_size > size || dib_width <= 0 || doubled_height <= 0 ||
			(doubled_height & 1) || ico_parser_u16(data + 12) != 1 ||
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
				masks[0] = ico_parser_u32(data + 40);
				masks[1] = ico_parser_u32(data + 44);
				masks[2] = ico_parser_u32(data + 48);
				if (header_size >= 56)
					masks[3] = ico_parser_u32(data + 52);
			}
			else
			{
				unsigned mask_count = compression == 6 ? 4 : 3;
				if ((size_t)mask_count * 4 > size - metadata_size)
					return false;
				for (unsigned i = 0; i < mask_count; ++i)
					masks[i] = ico_parser_u32(data + metadata_size + i * 4);
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
					uint32_t value = bits == 16 ? ico_parser_u16(row + (size_t)x * 2) : ico_parser_u32(row + (size_t)x * 4);
					r = ico_parser_channel(value, masks[0]);
					g = ico_parser_channel(value, masks[1]);
					b = ico_parser_channel(value, masks[2]);
					if (has_alpha)
						a = ico_parser_channel(value, masks[3]);
				}
				if (mask && ((mask[x / 8] >> (7 - (x & 7))) & 1))
					a = 0;
				output[(size_t)y * w + x] = (a << 24) | (b << 16) | (g << 8) | r;
			}
		}
		*width = w; *height = h; *pixels = output;
		return true;
	}
#endif
