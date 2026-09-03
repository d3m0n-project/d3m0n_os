#include "display.h"
#include "filesystem.h"
#include "memory.h"
#include "libft.h"

int	load_font(const char *path, t_font	*out)
{
	int	fd = open(path, O_READ);
	uint32_t	capacity;
	uint32_t	total_read;
	int			bytes_read;

	if (!out)
		return 1;
	ft_memset(out, 0, sizeof(*out));
	if (fd < 0)
	{
		log("Could not open font: %s\n", LOG_ERROR, path);
		return 1;
	}
	capacity = 4096;
	total_read = 0;
	out->data = kmalloc(capacity);
	if (!out->data)
	{
		close(fd);
		log("Could not allocate memory for font\n", LOG_ERROR);
		return 1;
	}
	while ((bytes_read = read(fd, (char *)(out->data + total_read), capacity - total_read)) > 0)
	{
		total_read += (uint32_t)bytes_read;
		if (total_read == capacity)
		{
			uint32_t	new_capacity = capacity * 2;
			uint8_t		*new_ptr = kmalloc(new_capacity);
			if (new_capacity <= capacity || !new_ptr)
			{
				kfree(out->data);
				out->data = 0;
				close(fd);
				log("Could not allocate memory for font\n", LOG_ERROR);
				return 1;
			}
			ft_memcpy(new_ptr, out->data, total_read);
			kfree(out->data);
			out->data = new_ptr;
			capacity = new_capacity;
		}
	}
	if (bytes_read < 0 || total_read == 0)
	{
		kfree(out->data);
		out->data = 0;
		close(fd);
		log("Could not read font: %s\n", LOG_ERROR, path);
		return 1;
	}
	out->size = total_read;
	if (ttf_parse(out) != 0)
	{
		kfree(out->data);
		out->data = 0;
		close(fd);
		log("Invalid or unsupported TrueType font: %s\n", LOG_ERROR, path);
		return 1;
	}
	log("Read %u bytes from %s!\n", LOG_SUCCESS, total_read, path);
	close(fd);
	return 0;
}

void	free_font(t_font *font)
{
	if (!font)
		return;
	kfree(font->data);
	font->data = 0;
}
