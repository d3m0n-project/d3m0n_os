#include "filesystem.h"
#include "bmp.h"
#include "memory.h"
#include "libft.h"
#include "time.h"
#include "random.h"

static uint8_t	bmp_extract_channel(uint32_t pixel, uint32_t mask)
{
	uint32_t	value;
	uint32_t	max;
	int			shift;

	if (!mask)
		return (255);

	shift = 0;
	while (((mask >> shift) & 1) == 0)
		shift++;

	value = (pixel & mask) >> shift;

	max = mask >> shift;
	while ((max & (max + 1)) != 0)
		max &= max + 1;
	max--;

	if (max == 0)
		return (0);

	return ((value * 255) / max);
}

int	bmp_load_image(BmpTexture *out, const char *path)
{
	#if SHOW_IMAGE_STATUS == 1
	uint64_t	start_time = time_us() / 1000;
	log("Loading BMP...\n", LOG_INFO);
	#endif

	int			fd = open(path, O_READ);
	char		header[54];
	char		*file_pixels = 0;
	int32_t		height;
	uint32_t	starting_offset;
	uint32_t	compression;
	uint32_t	row_size;
	uint32_t	stride;
	uint32_t	data_size;
	int			flip;
	uint32_t	red_mask = 0;
	uint32_t	green_mask = 0;
	uint32_t	blue_mask = 0;
	uint32_t	alpha_mask = 0;

	// this function may leak if alreay loaded pixels
	out->pixels = 0;

	if (fd == -1)
		return 1;

	rng_add_entropy(time_us()); // add entropy

	if (read(fd, header, 54) != 54)
		goto error;
	if (header[0] != 'B' || header[1] != 'M')
		goto error;

	ft_memcpy(&out->width, header + 18, sizeof(int));
	ft_memcpy(&height, header + 22, sizeof(int32_t));

	flip = (height > 0);
	out->height = (height > 0) ? height : -height;

	out->bytes_per_pixel = header[28] / 8;
	if (out->bytes_per_pixel != 3 && out->bytes_per_pixel != 4)
	{
		log("Invalid bmp bpp=%i only RGB or ARGB accepted\n", LOG_ERROR | LOG_INDENT, out->bytes_per_pixel);
		goto error;
	}


	ft_memcpy(&compression, header + 30, sizeof(uint32_t));
	if (compression != 0 && compression != 3 && compression != 6)
	{
		log("Unsupported BMP compression %u\n", LOG_ERROR | LOG_INDENT, compression);
		goto error;
	}

	ft_memcpy(&starting_offset, header + 10, sizeof(uint32_t));
	if (compression == 3 || compression == 6)
	{
		if (read(fd, (char *)&red_mask, 4) != 4)
			goto error;
		if (read(fd, (char *)&green_mask, 4) != 4)
			goto error;
		if (read(fd, (char *)&blue_mask, 4) != 4)
			goto error;

		if (out->bytes_per_pixel == 4)
		{
			if (read(fd, (char *)&alpha_mask, 4) != 4)
				alpha_mask = 0;
		}
	}
	else
	{
		// standard mask
		if (out->bytes_per_pixel == 3)
		{
			red_mask = 0x00FF0000;
			green_mask = 0x0000FF00;
			blue_mask = 0x000000FF;
			alpha_mask = 0;
		}
		else
		{
			red_mask = 0x00FF0000;
			green_mask = 0x0000FF00;
			blue_mask = 0x000000FF;
			alpha_mask = 0xFF000000;
		}
	}

	row_size = out->width * out->bytes_per_pixel;
	stride = (row_size + 3) & ~3;
	data_size = stride * out->height;

	file_pixels = malloc(data_size);
	if (!file_pixels)
	{
		log("Could not allocate %lu bytes for pixels\n", LOG_ERROR | LOG_INDENT, data_size);
		goto error;
	}

	#if SHOW_IMAGE_STATUS == 1
	log("BMP header loading: %llums\n", LOG_INDENT | LOG_INFO, (time_us() / 1000) - start_time);
	#endif

	lseek(fd, starting_offset, SEEK_SET);
	uint32_t	read_data_size = read(fd, file_pixels, data_size);
	if (read_data_size != data_size)
	{
		log("BMP read was invalid: needed %lu, got %lu\n", LOG_ERROR | LOG_INDENT, data_size, read_data_size);
		goto error;
	}

	out->pixels = malloc(out->width * out->height * sizeof(uint32_t));
	if (!out->pixels)
		goto error;

	#if SHOW_IMAGE_STATUS == 1
	log("BMP bpp: %i\n", LOG_INFO | LOG_INDENT, out->bytes_per_pixel);
	log("BMP size: %ix%i\n", LOG_INFO | LOG_INDENT, out->width, out->height);
	#endif

	for (int y = 0; y < out->height; y++)
	{
		uint8_t		*src;
		uint32_t	*dst;

		if (flip)
			src = (uint8_t *)file_pixels + (out->height - 1 - y) * stride;
		else
			src = (uint8_t *)file_pixels + y * stride;
		dst = out->pixels + y * out->width;
		for (int x = 0; x < out->width; x++)
		{
			uint32_t	pixel;
			uint8_t		r;
			uint8_t		g;
			uint8_t		b;
			uint8_t		a;

			if (out->bytes_per_pixel == 3)
			{
				b = src[0];
				g = src[1];
				r = src[2];
				a = 255;
				src += 3;
			}
			else
			{
				ft_memcpy(&pixel, src, 4);

				if (compression == 0)
				{
					b = src[0];
					g = src[1];
					r = src[2];
					a = src[3];
				}
				else
				{
					r = bmp_extract_channel(pixel, red_mask);
					g = bmp_extract_channel(pixel, green_mask);
					b = bmp_extract_channel(pixel, blue_mask);
					a = alpha_mask?bmp_extract_channel(pixel, alpha_mask):255;
				}
				src += 4;
			}
			*dst++ = ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
		}
	}

	#if SHOW_IMAGE_STATUS == 1
	log("total BMP loading time: %llums\n", LOG_INFO | LOG_INDENT, (time_us() / 1000) - start_time);
	#endif

	free(file_pixels);
	close(fd);
	return (0);

error:
	if (file_pixels)
		free(file_pixels);
	if (out->pixels)
		free(out->pixels);
	close(fd);
	return (1);
}

int	free_bmp_texture(BmpTexture *texture)
{
	if (texture->pixels)
		return (free(texture->pixels));
	if ((uintptr_t)texture >= (uintptr_t)HEAP_START && (uintptr_t)texture <= (uintptr_t)HEAP_END)
		free(texture);
	return 1;
}