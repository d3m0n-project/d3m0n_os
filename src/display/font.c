#include "display.h"
#include "filesystem.h"
#include "memory.h"

int	load_font(const char *path, t_font	*out, int dot_count)
{
	if (dot_count % 16 != 0)
	{
		log("Invalid dot count: %i\n", LOG_ERROR, dot_count);
		return 1;
	}
	int	fd = open(path, O_READ);
	if (fd < 0)
	{
		log("Could not open font: %s\n", LOG_ERROR, path);
		return 1;
	}
	uint32_t	size = 4096;
	uint32_t	total_read = 0;
	out->data = malloc(size);
	out->dot_count = dot_count;
	if (!out->data)
	{
		log("Could not allocate memory for font\n", LOG_ERROR);
		return 1;
	}
	while ((size = read(fd, (char *)(out->data + total_read), size)) > 0)
	{
		total_read += size;
		uint8_t	*new_ptr = malloc(total_read + size);
		if (!new_ptr)
		{
			free(out->data);
			close(fd);
			log("Could not allocate memory for font\n", LOG_ERROR);
			return 1;
		}
		memcpy(new_ptr, out->data, total_read);
		free(out->data);
		out->data = new_ptr;
	}
	log("Read %llu bytes from %s!\n", LOG_SUCCESS, total_read, path);
	close(fd);
	return 0;
}

void	free_font(t_font *font)
{
	free(font->data);
}