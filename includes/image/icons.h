#ifndef ICONS_H
#define ICONS_H

#include "bmp.h"
#include "libft.h"
#include "settings.h"

int			load_icon_pack(char *path);
BmpTexture	*get_icon(char *name, t_conf *conf);

#endif