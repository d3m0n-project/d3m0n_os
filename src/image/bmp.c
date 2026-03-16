#include "filesystem.h"
#include "bmp.h"
#include "memory.h"
#include "time.h"

int bmp_load_image(BmpTexture *out, const char *path)
{
	uint64_t start_time = time_us() / 1000;
	int fd = open(path, O_READ);
	log("loading BMP...\n", LOG_INFO);
	if (fd == -1)
		return 1;

	char header[54];
	if (read(fd, header, 54) != 54)
	{
		close(fd);
		return 1;
	}

	memcpy(&out->width, header + 18, sizeof(int));
	memcpy(&out->height, header + 22, sizeof(int));
	out->bytes_per_pixel = header[28] / 8;

	
	uint32_t starting_offset = 0;
	memcpy(&starting_offset, header + 10, sizeof(uint32_t));

	uint32_t row_size = out->width * out->bytes_per_pixel;
	uint32_t row_padding = (4 - (row_size % 4)) % 4;
	uint32_t stride = row_size + row_padding;

	uint32_t data_size = stride * out->height;

	char *file_pixels = malloc(data_size);
	if (!file_pixels)
	{
		close(fd);
		return 1;
	}

	lseek(fd, starting_offset, SEEK_SET);

	if (read(fd, file_pixels, data_size) != data_size)
	{
		free(file_pixels);
		close(fd);
		return 1;
	}

	out->pixels = malloc(out->width * out->height * sizeof(uint32_t));
	if (!out->pixels)
	{
		free(file_pixels);
		close(fd);
		return 1;
	}

	for (int y = 0; y < out->height; y++)
	{
		char *row = file_pixels + (out->height - 1 - y) * stride;
		uint32_t row_start = y * out->width;

		for (int x = 0; x < out->width; x++)
		{
			if (out->bytes_per_pixel == 3)
			{
				unsigned char *p = (unsigned char *)row + x * 3;
				out->pixels[row_start + x] =
					(p[0] << 16) |
					(p[1] << 8)  |
					(p[2]);
			}
			else
			{
				unsigned char *p = (unsigned char *)row + x * 4;
				out->pixels[row_start + x] =
					(p[0] << 16) |
					(p[1] << 8)  |
					(p[2]) |
					(p[3] << 24);
			}
		}
	}

	log("total BMP loading time: %llums\n", LOG_INFO, (time_us() / 1000) - start_time);

	free(file_pixels);
	close(fd);
	return 0;
}

int	free_bmp_texture(BmpTexture *texture)
{
	return (free(texture->pixels));
}