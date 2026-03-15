#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"

#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	320

int		display_init();
void	put_pixel(int x, int y, uint32_t color);
int		framebuffer_init(uint32_t width, uint32_t height, uint32_t bpp);

#endif