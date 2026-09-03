#ifndef FONT_HPP
#define FONT_HPP

#include "types.h"
#include "sys.h"
#include "stdio.hpp"
#include "memory.h"
#include "stdlib.h"
#include "exception"
#include "string.hpp"
#include "ttf.h"

using namespace std;

static inline int	load_font(const char *path, t_font	*out)
{
	int	fd = open(path, O_READ);
	uint32_t	capacity;
	uint32_t	total_read;
	int			bytes_read;

	if (!out)
		return 1;
	memset(out, 0, sizeof(*out));
	if (fd < 0)
	{
		throw AppException("Could not open font");
		return 1;
	}
	capacity = 4096;
	total_read = 0;
	out->data = (uint8_t *)malloc(capacity);
	if (!out->data)
	{
		close(fd);
		throw AppException("Could not allocate memory for font");
		return 1;
	}
	while ((bytes_read = read(fd, (char *)(out->data + total_read), capacity - total_read)) > 0)
	{
		total_read += (uint32_t)bytes_read;
		if (total_read == capacity)
		{
			uint32_t	new_capacity = capacity * 2;
			uint8_t		*new_ptr = (uint8_t *)malloc(new_capacity);
			if (new_capacity <= capacity || !new_ptr)
			{
				free(out->data);
				out->data = 0;
				close(fd);
				throw AppException("Could not allocate memory for font");
				return 1;
			}
			memcpy(new_ptr, out->data, total_read);
			free(out->data);
			out->data = new_ptr;
			capacity = new_capacity;
		}
	}
	if (bytes_read < 0 || total_read == 0)
	{
		free(out->data);
		out->data = 0;
		close(fd);
		throw AppException("Could not read font");
		return 1;
	}
	out->size = total_read;
	if (ttf_parse(out) != 0)
	{
		free(out->data);
		out->data = 0;
		close(fd);
		throw AppException("Invalid or unsupported TrueType font");
	}
	printf("Read %u bytes from %s!\n", total_read, path);
	close(fd);
	return 0;
}

static inline void	free_font(t_font *font)
{
	if (!font)
		return;
	free(font->data);
	font->data = 0;
}

#endif
