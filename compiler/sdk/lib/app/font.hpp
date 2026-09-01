#ifndef FONT_HPP
#define FONT_HPP

#include "types.h"
#include "sys.h"
#include "stdio.hpp"
#include "memory.h"
#include "exception"
#include "string.hpp"

using namespace std;

typedef struct s_font
{
	int		dot_count;
	char	name[25];
	uint8_t	*data;
}	t_font;

static inline int	load_font(const char *path, t_font	*out, int dot_count)
{
	int	fd = open(path, O_READ);
	if (fd < 0)
	{
		throw AppException((string("Could not open font: ") + string(path)).c_str());
		return 1;
	}
	uint32_t	size = 4096;
	uint32_t	total_read = 0;
	out->data = (uint8_t *)malloc(size);
	out->dot_count = dot_count;
	if (!out->data)
	{
		printf("Could not allocate memory for font\n");
		return 1;
	}
	while ((size = read(fd, (char *)(out->data + total_read), size)) > 0)
	{
		total_read += size;
		uint8_t	*new_ptr = (uint8_t *)malloc(total_read + size);
		if (!new_ptr)
		{
			free(out->data);
			close(fd);
			throw AppException("Could not allocate memory for font\n");
			return 1;
		}
		memcpy(new_ptr, out->data, total_read);
		free(out->data);
		out->data = new_ptr;
	}
	printf("Read %u bytes from %s!\n", total_read, path);
	close(fd);
	return 0;
}

static inline void	free_font(t_font *font)
{
	free(font->data);
}

#endif