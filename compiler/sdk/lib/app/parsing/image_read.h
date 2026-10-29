#ifndef IMAGE_READ_H
#define IMAGE_READ_H

#include "sys.h"


static inline char	*image_read_to_buf(const char *path, size_t *buf_size)
{
	int fd = open(path, O_READ);
	if (fd < 0)
		return 0;
	char *svg = (char *)malloc(SVG_BUFFER_LIMIT + 1);
	if (!svg)
	{
		close(fd);
		return 1;
	}
	int total = 0, amount;
	while (total < SVG_BUFFER_LIMIT && (amount = read(fd, svg + total, SVG_BUFFER_LIMIT - total)) > 0)
		total += amount;
	close(fd);
	if (!total)
	{
		free(svg);
		return 1;
	}
	svg[total] = 0;
	draw_svg_buff(x, y, w, h, svg, total, override_color);
	free(svg);
	return 0;
}

#endif