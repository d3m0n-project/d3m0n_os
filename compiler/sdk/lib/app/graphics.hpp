#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "types.h"
#include <sys.h>
#include "stdio.hpp"

class Display
{
private:
	uint8_t	*fb;
	int		pitch;

	void	draw_ellipse_points(int cx, int cy, int x, int y, uint32_t color)
	{
		this->put_pixel(cx + x, cy + y, color);
		this->put_pixel(cx - x, cy + y, color);
		this->put_pixel(cx + x, cy - y, color);
		this->put_pixel(cx - x, cy - y, color);
	}
public:
	int		w = 0;
	int		h = 0;
	Display()
	{
		write(1, "c\n", 2);
		getfbaddr(&this->fb, &this->w, &this->h, &this->pitch);
		write(1, "d\n", 2);
		//printf("Created a w=%i h=%i pitch=%i display\n", this->w, this->h, this->pitch);
	}

	void		put_pixel(int x, int y, uint32_t color);
	uint32_t	get_pixel(int x, int y);
	void		draw_hline(int x, int y, int w, uint32_t color);
	void		draw_rect(int x, int y, int w, int h, uint32_t color);
	void		draw_ellipse(int cx, int cy, int rx, int ry, uint32_t color, int filled);
};

#endif