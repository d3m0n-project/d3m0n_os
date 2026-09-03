#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "types.h"
#include "sys.h"
#include "stdio.hpp"
#include "font.hpp"
#include "string.hpp"
#include "stdlib.h"
#include <exception>

using namespace std;


class Display
{
private:
	uint8_t	*fb;
	int		pitch;
	t_font	main_font;

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
		if (load_font("/fonts/Inter.ttf", &this->main_font))
			throw AppException("Could not initialize main font!");
		this->w = 320;
		this->h = 480 - 32; // change according to topbar height
		if (surface_create(this->w, this->h, &this->fb, &this->pitch))
			throw AppException("Could not initialize display surface!");
	}

	void		present(void) { surface_update(this->fb); }

	void		put_pixel(int x, int y, uint32_t color);
	uint32_t	get_pixel(int x, int y);
	void		draw_hline(int x, int y, int w, uint32_t color);
	void		draw_rect(int x, int y, int w, int h, uint32_t color);
	void		draw_ellipse(int cx, int cy, int rx, int ry, uint32_t color, int filled);
	void		draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font	*font);
	void		draw_text_at(int x, int y, int font_size, const char *text, uint32_t color, t_font *font = 0);
	void		draw_svg(int x, int y, int w, int h, const char *path, uint32_t override_color = 0);
};

#endif
