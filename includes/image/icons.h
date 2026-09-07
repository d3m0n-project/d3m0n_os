#ifndef ICONS_H
#define ICONS_H

#include "bmp.h"
#include "libft.h"
#include "settings.h"

typedef enum
{
	ICON_FORMAT_UNKNOWN,
	ICON_FORMAT_BMP,
	ICON_FORMAT_SVG
}	e_icon_format;

typedef struct s_svg
{
	void	*data;
	size_t	size;
}	t_svg;

int			load_icon_pack(char *path);
void		*get_icon(char *name, t_conf *conf, e_icon_format *type);

int			svg_read_file(struct s_svg *output, const char *path);

void		draw_icon(int x, int y, int w, int h, const char *icon_name, uint32_t override_color);

#endif