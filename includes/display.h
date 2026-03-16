#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "bmp.h"
#include "color.h"

#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	320

extern uint32_t	DISPLAY_COLORS[16];

int		display_init();
void	put_pixel(int x, int y, uint32_t color);
void	draw_rect(int x, int y, int w, int h, uint32_t color);
void	draw_bmp(int x, int y, int w, int h, BmpTexture *texture);
int		framebuffer_init(uint32_t width, uint32_t height, uint32_t bpp);

#endif