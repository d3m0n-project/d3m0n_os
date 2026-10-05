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


/** @brief Represents the Display type. */
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
			/** @brief AppException operation. */
			throw AppException("Could not initialize main font!");
		this->w = 320;
		this->h = 480 - 32; // change according to topbar height
		if (surface_create(this->w, this->h, &this->fb, &this->pitch))
			/** @brief AppException operation. */
			throw AppException("Could not initialize display surface!");
	}

	/** @brief present operation. */
	void		present(void) { surface_update(this->fb); }

	/** @brief put_pixel operation. */
	void		put_pixel(int x, int y, uint32_t color);
	/** @brief get_pixel operation. */
	uint32_t	get_pixel(int x, int y);
	/** @brief draw_hline operation. */
	void		draw_hline(int x, int y, int w, uint32_t color);
	/** @brief draw_rect operation. */
	void		draw_rect(int x, int y, int w, int h, uint32_t color);
	/** @brief draw_rounded_rect operation. */
	void		draw_rounded_rect(int x, int y, int w, int h, int radius, uint32_t color);
	/** @brief draw_ellipse operation. */
	void		draw_ellipse(int cx, int cy, int rx, int ry, uint32_t color, int filled);
	/** @brief draw_text operation. */
	void		draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font	*font);
	/** @brief draw_text_at operation. */
	void		draw_text_at(int x, int y, int font_size, const char *text, uint32_t color, t_font *font = 0);

	/** @brief draw_svg operation. */
	int			draw_svg(int x, int y, int w, int h, const char *path, uint32_t override_color = 0);
	/** @brief draw_svg_buff operation. */
	void		draw_svg_buff(int x, int y, int w, int h, const char *data, size_t size, uint32_t override_color = 0);
};

#endif
