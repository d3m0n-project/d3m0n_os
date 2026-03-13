#include "display.h"

#define FB ((volatile uint32_t*)0x40002000)
#define WIDTH	800
#define HEIGHT	600

void put_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
		return;
	FB[y * WIDTH + x] = color;
}
