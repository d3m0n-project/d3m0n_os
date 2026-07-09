#ifndef ICONS_H
#define ICONS_H

#include "bmp.h"
#include "libft.h"

int			load_icon_pack(char *path);
BmpTexture	*get_icon(char *name);

#endif