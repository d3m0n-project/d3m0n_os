#include "ico.h"

typedef struct
{
	uint32_t	size;
	int32_t		width;
	int32_t		height;
	uint16_t	planes;
	uint16_t	bit_count;
	uint32_t	compression;
	uint32_t	size_image;
	int32_t		x_pixels_per_meter;
	int32_t		y_pixels_per_meter;
	uint32_t	colors_used;
	uint32_t	colors_important;
}	BITMAPINFOHEADER;

int	load_dib_from_buff(char *raw, size_t size, ICO_icon *out)
{
	BITMAPINFOHEADER header = {0};

	if (!raw || !out || size < sizeof(header))
		return 1;

	ft_memcpy(&header, raw, sizeof(header));

	if (header.size != 40 || header.planes != 1 || header.bit_count != 32 || header.compression != 0)
		return 1;

	uint32_t width = header.width;
	uint32_t height = header.height / 2;
	size_t row_size = width * 4;
	size_t pixel_size = row_size * height;

	if (!width || !height || sizeof(uint32_t) * (size_t)width * height > SIZE_MAX)
		return 1;

	if (header.size + pixel_size > size)
		return 1;

	uint32_t *pixels = kmalloc(width * height * sizeof(uint32_t));
	if (!pixels)
		return 1;

	const uint8_t *src = (const uint8_t *)raw + header.size;

	for (uint32_t y = 0; y < height; y++)
	{
		const uint8_t *row = src + (height - 1 - y) * row_size;
		for (uint32_t x = 0; x < width; x++)
		{
			const uint8_t *p = row + x * 4;
			pixels[y * width + x] = ((uint32_t)p[3] << 24) | ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
		}
	}

	out->pixels = (uint32_t)(uintptr_t)pixels;
	out->width = width;
	out->height = height;
	return 0;
}