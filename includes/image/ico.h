#ifndef ICO_H
#define ICO_H

#include "types.h"
#include "libft.h"
#include "log.h"

typedef struct ICO_icon
{
	uint32_t pixels;
	uint32_t width;
	uint32_t height;
}	ICO_icon;

int		load_ico_from_buff(char *raw, size_t size, ICO_icon *out);
int		load_dib_from_buff(char *raw, size_t size, ICO_icon *out);

#endif