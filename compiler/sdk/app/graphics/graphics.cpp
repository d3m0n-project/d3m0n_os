#include "app/graphics.hpp"
#include "app/font.hpp"

void	Display::put_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || (uint32_t)x >= (uint32_t)this->w || y < 0 || (uint32_t)y >= (uint32_t)this->h)
		return;
	if ((color & 0xFF000000) == 0x00000000) // TODO: real transparency
		return;
	volatile uint32_t *framebuffer = (volatile uint32_t *)this->fb;
	framebuffer[y * (this->pitch / 4) + x] = color;
}

uint32_t	Display::get_pixel(int x, int y)
{
	if (x < 0 || (uint32_t)x >= (uint32_t)this->w || y < 0 || (uint32_t)y >= (uint32_t)this->h)
		return 0;
	volatile uint32_t *framebuffer = (volatile uint32_t *)this->fb;
	return framebuffer[y * (this->pitch / 4) + x];
}

void	Display::draw_hline(int x, int y, int w, uint32_t color)
{
	if (w < 0)
	{
		w = -w;
		x -= w;
	}
	for (int i=0; i<w; i++)
		put_pixel(x + i, y, color);
}

void	Display::draw_rect(int x, int y, int w, int h, uint32_t color)
{
	for(int j=0; j<h; j++)
		this->draw_hline(x, y + j, w, color);
}

void	Display::draw_ellipse(int cx, int cy, int rx, int ry, uint32_t color, int filled)
{
	int		x;
	int		y;
	long	rx2;
	long	ry2;
	long	two_rx2;
	long	two_ry2;
	long	d1;
	long	d2;
	long	dx;
	long	dy;

	if (rx <= 0 || ry <= 0)
		return;

	x = 0;
	y = ry;
	rx2 = (long)rx * rx;
	ry2 = (long)ry * ry;
	two_rx2 = 2 * rx2;
	two_ry2 = 2 * ry2;

	dx = 0;
	dy = two_rx2 * y;

	// region 1
	d1 = ry2 - (rx2 * ry) + (rx2 / 4);
	while (dx < dy)
	{
		if (filled)
		{
			this->draw_hline(cx - x, cy + y, 2 * x + 1, color);
			this->draw_hline(cx - x, cy - y, 2 * x + 1, color);

			if (x != y)
			{
				this->draw_hline(cx - y, cy + x, 2 * y + 1, color);
				this->draw_hline(cx - y, cy - x, 2 * y + 1, color);
			}
		}
		else
			this->draw_ellipse_points(cx, cy, x, y, color);

		if (d1 < 0)
		{
			x++;
			dx += two_ry2;
			d1 += dx + ry2;
		}
		else
		{
			x++;
			y--;
			dx += two_ry2;
			dy -= two_rx2;
			d1 += dx - dy + ry2;
		}
	}

	// region 2
	d2 = ry2 * (x * x + x) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;
	while (y >= 0)
	{
		if (filled)
		{
			this->draw_hline(cx - x, cy + y, 2 * x + 1, color);
			this->draw_hline(cx - x, cy - y, 2 * x + 1, color);
			if (x != y)
			{
				this->draw_hline(cx - y, cy + x, 2 * y + 1, color);
				this->draw_hline(cx - y, cy - x, 2 * y + 1, color);
			}
		}
		else
			this->draw_ellipse_points(cx, cy, x, y, color);

		if (d2 > 0)
		{
			y--;
			dy -= two_rx2;
			d2 += rx2 - dy;
		}
		else
		{
			y--;
			x++;
			dx += two_ry2;
			dy -= two_rx2;
			d2 += dx - dy + rx2;
		}
	}
}

typedef struct s_font_context
{
	Display		*display;
	uint32_t	color;
} t_font_context;

static void	font_pixel(void *context, int x, int y, uint8_t coverage)
{
	(void)coverage;
	t_font_context *font_context = (t_font_context *)context;
	font_context->display->put_pixel(x, y, font_context->color);
}

void	Display::draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font	*font)
{
	if (!text)
		return;
	if (!font)
		font = &main_font;
	if (!font || !font->data || !font->units_per_em || h <= 0)
		return;
	int cursor_x = x;
	int cursor_y = y;
	int line_height = h;
	int baseline = y + ((int)font->ascender * h) /
		(font->ascender - font->descender);
	(void)w;
	t_font_context font_context = {this, color};
	for (uint32_t i = 0; text[i]; i++)
	{
		if (text[i] == '\n')
		{
			cursor_x = x;
			cursor_y += line_height;
			baseline = cursor_y + ((int)font->ascender * h) /
				(font->ascender - font->descender);
			continue;
		}
		uint16_t glyph = ttf_glyph_for_codepoint(font, (uint8_t)text[i]);
		ttf_render_glyph(font, glyph, cursor_x, baseline, h, h, font_pixel,
			&font_context);
		cursor_x += (ttf_glyph_advance(font, glyph) * h) /
			font->units_per_em;
	}
}

static int	text_width_for_size(const char *text, int font_size, t_font *font)
{
	int			line_width;
	int			max_width;
	uint16_t	glyph;

	if (!text || !font || !font->data || !font->units_per_em || font_size <= 0)
		return 0;
	line_width = 0;
	max_width = 0;
	for (uint32_t i = 0; text[i]; i++)
	{
		if (text[i] == '\n')
		{
			if (line_width > max_width)
				max_width = line_width;
			line_width = 0;
			continue;
		}
		glyph = ttf_glyph_for_codepoint(font, (uint8_t)text[i]);
		line_width += (ttf_glyph_advance(font, glyph) * font_size) / font->units_per_em;
	}
	if (line_width > max_width)
		max_width = line_width;
	return max_width;
}

void	Display::draw_text_at(int x, int y, int font_size, const char *text, uint32_t color, t_font *font)
{
	int	width;

	if (!font)
		font = &main_font;
	width = text_width_for_size(text, font_size, font);
	if (width <= 0)
		return;
	draw_text(x, y, width, font_size, text, color, font);
}
