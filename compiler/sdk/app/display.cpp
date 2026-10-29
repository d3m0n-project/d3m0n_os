#include "app/graphics.hpp"
#include "app/font.hpp"
#include "app/parsing/svg.h"

#define IS_TRANSPARENT(c)			((c & 0xFF000000) == 0x00000000)
#define BLEND_CHANNEL(f, b, a)		(((((unsigned)(f) * (a) + (unsigned)(b) * (255 - (a))) * 257) + 257) >> 16)

int Display::draw_png(int x, int y, int w, int h, char *data, size_t data_size, t_png_cache *cache)
{
	if (w <= 0 || h <= 0 || !data)
		return 1;

	if (!cache->pixels)
	{
		if (!png_decode((unsigned char *)data, data_size, &cache->image_width, &cache->image_height, &cache->pixels))
		{
			fprintf(stderr, "draw_png: Failed to decode png, may be invalid or unsupported\n");
			return 1;
		}
	}

	uint32_t			*framebuffer = (uint32_t *)this->fb;
	int					bpp = this->pitch / 4;
	unsigned			iw = cache->image_width;
	unsigned			ih = cache->image_height;
	unsigned			uw = (unsigned)w;
	unsigned			uh = (unsigned)h;

	unsigned			y_acc = 0;
	unsigned			y_base = ih / uh;
	unsigned			y_rem = ih % uh;
	const uint32_t		*src_row = cache->pixels;
	for (int py = 0; py < h; py++)
	{
		uint32_t		*dst = framebuffer + (size_t)(y + py) * bpp + x;
		unsigned		x_acc = 0;
		unsigned		x_base = iw / uw;
		unsigned		x_rem = iw % uw;
		const uint32_t	*src = src_row;

		for (int px = 0; px < w; px++)
		{
			uint32_t color = *src;
			unsigned alpha = color >> 24;
			if (alpha)
			{
				if (alpha < 255)
				{
					uint32_t bg = *dst;
					color = 0xff000000
						| (BLEND_CHANNEL((color >> 16) & 0xff, (bg >> 16) & 0xff, alpha) << 16)
						| (BLEND_CHANNEL((color >> 8) & 0xff, (bg >> 8) & 0xff, alpha) << 8)
						| BLEND_CHANNEL(color & 0xff, bg & 0xff, alpha);
				}
				*dst = color;
			}
			dst++;
			src += x_base;
			x_acc += x_rem;
			if (x_acc >= uw)
			{
				x_acc -= uw;
				src++;
			}
		}

		src_row += (size_t)y_base * iw;
		y_acc += y_rem;
		if (y_acc >= uh)
		{
			y_acc -= uh;
			src_row += iw;
		}
	}
	return 0;
}

void	Display::put_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || (uint32_t)x >= (uint32_t)this->w || y < 0 || (uint32_t)y >= (uint32_t)this->h)
		return;
	if (IS_TRANSPARENT(color))
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
	if (IS_TRANSPARENT(color))
		return;

	if (w < 0)
	{
		w = -w;
		x -= w;
	}
	int bpp = this->pitch / 4;
	uint32_t *framebuffer = (uint32_t *)this->fb;
	uint32_t *p = framebuffer + y * bpp + x;
	uint64_t pair = ((uint64_t)color << 32) | color;
	uint32_t *end = p + w;
	/* 64-bit stores need 8-byte alignment on ARMv6 (strd faults otherwise) */
	while (p < end && ((uintptr_t)p & 7))
		*p++ = color;
	for (; p + 1 < end; p += 2)
		*(volatile uint64_t *)(uintptr_t)p = pair;
	while (p < end)
		*p++ = color;
}

void	Display::draw_rect(int x, int y, int w, int h, uint32_t color)
{
	if (IS_TRANSPARENT(color))
		return;
	int bpp = this->pitch / 4;
	uint32_t *framebuffer = (uint32_t *)this->fb;
	uint32_t *row = framebuffer + y * bpp + x;
	uint64_t pair = ((uint64_t)color << 32) | color;
	int step = bpp - w;   /* remaining pixels to skip to reach next row */
	for (int j = 0; j < h; j++)
	{
		uint32_t *p = row;
		uint32_t *end = p + w;
		/* 64-bit stores need 8-byte alignment on ARMv6 (strd faults otherwise) */
		while (p < end && ((uintptr_t)p & 7))
			*p++ = color;
		/* write 2 pixels per 64-bit store */
		for (; p + 1 < end; p += 2)
			*(volatile uint64_t *)(uintptr_t)p = pair;
		while (p < end)
			*p++ = color;
		row += w + step;
	}
}

void	Display::draw_rounded_rect(int x, int y, int w, int h, int radius, uint32_t color)
{
	if (w <= 0 || h <= 0)
		return;

	if (IS_TRANSPARENT(color))
		return;

	if (radius < 0)
		radius = 0;

	if (radius > w / 2)
		radius = w / 2;
	if (radius > h / 2)
		radius = h / 2;

	int x2 = x + w - 1;
	int y2 = y + h - 1;
	for (int py = y; py <= y2; py++)
	{
		for (int px = x; px <= x2; px++)
		{
			int cx = px;
			int cy = py;

			if (px < x + radius && py < y + radius)
			{
				cx = x + radius;
				cy = y + radius;
			}
			else if (px >= x2 - radius && py < y + radius)
			{
				cx = x2 - radius;
				cy = y + radius;
			}
			else if (px < x + radius && py >= y2 - radius)
			{
				cx = x + radius;
				cy = y2 - radius;
			}
			else if (px >= x2 - radius && py >= y2 - radius)
			{
				cx = x2 - radius;
				cy = y2 - radius;
			}
			else
			{
				this->put_pixel(px, py, color);
				continue;
			}

			int dx = px - cx;
			int dy = py - cy;

			if (dx * dx + dy * dy <= radius * radius)
				this->put_pixel(px, py, color);
		}
	}
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

	if (IS_TRANSPARENT(color))
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
	t_font_context *font_context = (t_font_context *)context;
	uint32_t	bg = font_context->display->get_pixel(x, y);
	uint32_t	fg = font_context->color;


	uint32_t r = (((fg >> 16) & 0xff) * coverage + ((bg >> 16) & 0xff) * (255 - coverage)) / 255;
	uint32_t g = (((fg >> 8) & 0xff) * coverage + ((bg >> 8) & 0xff) * (255 - coverage)) / 255;
	uint32_t b = ((fg & 0xff) * coverage + (bg & 0xff) * (255 - coverage)) / 255;

	uint32_t dst_color = 0xFF000000 | (r << 16) | (g << 8) | b;

	
	font_context->display->put_pixel(x, y, dst_color);
}

void	Display::draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font *font)
{
	t_font_context	font_context;
	int				cursor_x;
	int				cursor_y;
	int				line_height;
	int				baseline;

	if (IS_TRANSPARENT(color))
		return;

	if (!text)
		return;
	
	if (!font)
		font = &main_font;
	
	if (!font || !font->data || !font->units_per_em || h <= 0)
		return;

	cursor_x = x;
	cursor_y = y;
	line_height = h;
	baseline = y + ((int)font->ascender * h) / (font->ascender - font->descender);
	(void)w;

	font_context.display = this;
	font_context.color = color;
	for (uint32_t i = 0; text[i]; i++)
	{
		if (text[i] == '\n')
		{
			cursor_x = x;
			cursor_y += line_height;
			baseline = cursor_y + ((int)font->ascender * h) / (font->ascender - font->descender);
			continue ;
		}

		uint16_t glyph;
		glyph = ttf_glyph_for_codepoint(font, (uint8_t)text[i]);
		//printf("char='%c' cp=%u glyph=%u advance=%d\n",
		//	text[i],
		//	(uint8_t)text[i],
		//	glyph,
		//	ttf_glyph_advance(font, glyph));
		ttf_render_glyph(font, glyph, cursor_x, baseline, h, h, font_pixel, &font_context);
		cursor_x += (ttf_glyph_advance(font, glyph) * h) / font->units_per_em;
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

int	Display::text_width(int font_size, const char *text, t_font *font)
{
	if (!font)
		font = &main_font;
	return text_width_for_size(text, font_size, font);
}

void	Display::draw_text_at(int x, int y, int font_size, const char *text, uint32_t color, t_font *font)
{
	int	width;

	if (IS_TRANSPARENT(color))
		return;

	if (!font)
		font = &main_font;
	width = text_width_for_size(text, font_size, font);
	if (width <= 0)
		return;
	draw_text(x, y, width, font_size, text, color, font);
}
