#include "filesystem.h"
#include "bmp.h"
#include "memory.h"
#include "libft.h"
#include "time.h"

int	bmp_load_image(BmpTexture *out, const char *path)
{
	#if SHOW_IMAGE_STATUS == 1
	uint64_t	start_time = time_us() / 1000;
	log("loading BMP...\n", LOG_INFO);
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

	if (fd == -1)
		return 1;

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
	if (compression != 0)
	{
		log("BMP compression is enabled and it is not yet handled\n", LOG_ERROR | LOG_INDENT);
		goto error;
	}

	ft_memcpy(&starting_offset, header + 10, sizeof(uint32_t));

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

	uint64_t t = time_us();

	log("offset=%u\n", LOG_INFO | LOG_INDENT, starting_offset);
	log("width=%d height=%d bpp=%d\n", LOG_INDENT | LOG_INFO, out->width, out->height, out->bytes_per_pixel);

	lseek(fd, starting_offset, SEEK_SET);

	log("seek: %llums\n", LOG_INFO | LOG_INDENT, (time_us() - t)/1000);

	t = time_us();
	uint32_t	read_data_size = read(fd, file_pixels, data_size);
	if (read_data_size != data_size)
	{
		log("BMP read was invalid: needed %lu, got %lu\n", LOG_ERROR | LOG_INDENT, data_size, read_data_size);
		goto error;
	}

	log("read: %llums\n", LOG_INFO | LOG_INDENT, (time_us() - t)/1000);


	t = time_us();
	out->pixels = malloc(out->width * out->height * sizeof(uint32_t));
	log("malloc: %llums\n", LOG_INFO | LOG_INDENT, (time_us() - t)/1000);

	if (!out->pixels)
		goto error;

	#if SHOW_IMAGE_STATUS == 1
	log("BMP bpp: %i\n", LOG_INFO | LOG_INDENT, out->bytes_per_pixel);
	log("BMP size: %ix%i\n", LOG_INFO | LOG_INDENT, out->width, out->height);
	#endif

	t = time_us();
	if (out->bytes_per_pixel == 3)
	{
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
				*dst++ = (0xFFu << 24) | (src[0] << 16) | (src[1] << 8) | src[2];
				src += 3;
			}
		}
	}
	else
	{
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
				*dst++ = (src[0] << 16) | (src[1] << 8) | src[2] | (src[3] << 24);
				src += 4;
			}
		}
	}
	log("convert: %llums\n", LOG_INFO | LOG_INDENT, (time_us() - t)/1000);

	#if SHOW_IMAGE_STATUS == 1
	log("total BMP loading time: %llums\n", LOG_INFO | LOG_INDENT, (time_us() / 1000) - start_time);
	#endif

	free(file_pixels);
	close(fd);
	return (0);

error:
	free(file_pixels);
	free(out->pixels);
	close(fd);
	return (1);
}

int	free_bmp_texture(BmpTexture *texture)
{
	if (texture->pixels)
		return (free(texture->pixels));
	return 1;
}