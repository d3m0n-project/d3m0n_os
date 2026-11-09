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
	int step = bpp - w;
	for (int j = 0; j < h; j++)
	{
		uint32_t *p = row;
		uint32_t *end = p + w;
		while (p < end && ((uintptr_t)p & 7))
			*p++ = color;
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
	Display	 *display;
	uint32_t	color;
} t_font_context;


#define GLYPH_CACHE_SIZE 128

typedef struct s_glyph_pixel
{
	int32_t	 		x;
	int32_t	 		y;
	uint8_t	 		coverage;
} t_glyph_pixel;

typedef struct s_glyph_entry
{
	const t_font	 *font;
	uint16_t		 glyph;
	int			  	size;
	t_glyph_pixel	*pixels;
	uint32_t		 count;
	uint32_t		 capacity;
	uint8_t		 	 valid;
} t_glyph_entry;

typedef struct s_glyph_capture
{
	t_glyph_entry   *entry;
	int				 origin_x;
	int				 origin_y;
	int				 failed;
} t_glyph_capture;

static t_glyph_entry g_glyph_cache[GLYPH_CACHE_SIZE] = {};
static uint32_t g_glyph_cache_next = 0;


static inline uint32_t blend_channel(uint32_t fg, uint32_t bg, uint32_t a, uint32_t ia)
{
	uint32_t v = fg * a + bg * ia;
	return (v + 1 + (v >> 8)) >> 8;
}


static inline void blend_font_pixel(Display *display, uint32_t fg, int x, int y, uint8_t coverage)
{
	if (!coverage)
		return;

	if (x < 0 || y < 0 || x >= display->w || y >= display->h)
		return;

	volatile uint32_t *fb = (volatile uint32_t *)display->fb;

	uint32_t index = (uint32_t)y * (display->pitch / 4) + (uint32_t)x;
	uint32_t bg = fb[index];

	uint32_t a = coverage;
	uint32_t ia = 255 - a;

	uint32_t r = blend_channel((fg >> 16) & 255, (bg >> 16) & 255, a, ia);
	uint32_t g = blend_channel((fg >> 8) & 255, (bg >> 8) & 255, a, ia);
	uint32_t b = blend_channel(fg & 255, bg & 255, a, ia);
	fb[index] = 0xFF000000 | (r << 16) | (g << 8) | b;
}


static void font_pixel(void *context, int x, int y, uint8_t coverage)
{
	t_font_context *fc = (t_font_context *)context;
	blend_font_pixel(fc->display, fc->color, x, y, coverage);
}


static t_glyph_entry *find_cached_glyph(const t_font *font, uint16_t glyph, int size)
{
	for (uint32_t i = 0; i < GLYPH_CACHE_SIZE; i++)
	{
		t_glyph_entry *e = &g_glyph_cache[i];

		if (e->valid && e->font == font && e->glyph == glyph && e->size == size)
			return e;
	}
	return NULL;
}

static void capture_glyph_pixel(void *context, int x, int y, uint8_t coverage)
{
	t_glyph_capture *capture = (t_glyph_capture *)context;
	if (!coverage || capture->failed)
		return;

	t_glyph_entry *e = capture->entry;
	if (e->count >= e->capacity)
	{
		uint32_t new_capacity = e->capacity ? e->capacity * 2 : 128;
		if (new_capacity < e->capacity || new_capacity > SIZE_MAX / sizeof(t_glyph_pixel))
		{
			capture->failed = 1;
			return;
		}

		t_glyph_pixel *new_pixels = (t_glyph_pixel *)realloc(e->pixels, (size_t)new_capacity * sizeof(t_glyph_pixel));
		if (!new_pixels)
		{
			capture->failed = 1;
			return;
		}

		e->pixels = new_pixels;
		e->capacity = new_capacity;
	}

	t_glyph_pixel *p = &e->pixels[e->count++];
	p->x = x - capture->origin_x;
	p->y = y - capture->origin_y;
	p->coverage = coverage;
}

void clear_glyph_cache(void)
{
	for (uint32_t i = 0; i < GLYPH_CACHE_SIZE; i++)
	{
		free(g_glyph_cache[i].pixels);
		memset(&g_glyph_cache[i], 0, sizeof(g_glyph_cache[i]));
	}
	g_glyph_cache_next = 0;
}

static t_glyph_entry *allocate_glyph_entry(void)
{
	t_glyph_entry *e = &g_glyph_cache[g_glyph_cache_next];
	g_glyph_cache_next++;

	if (g_glyph_cache_next >= GLYPH_CACHE_SIZE)
		g_glyph_cache_next = 0;

	free(e->pixels);
	memset(e, 0, sizeof(*e));
	return e;
}


static void draw_cached_glyph(t_font_context *fc, const t_font *font, uint16_t glyph, int x, int baseline, int size)
{
	t_glyph_entry *e = find_cached_glyph(font, glyph, size);
	if (!e)
	{
		e = allocate_glyph_entry();

		e->font = font;
		e->glyph = glyph;
		e->size = size;

		t_glyph_capture capture;

		capture.entry = e;
		capture.origin_x = x;
		capture.origin_y = baseline;
		capture.failed = 0;

		ttf_render_glyph(font, glyph, x, baseline, size, size, capture_glyph_pixel, &capture);
		if (capture.failed)
		{
			free(e->pixels);
			memset(e, 0, sizeof(*e));
			ttf_render_glyph(font, glyph, x, baseline, size, size, font_pixel, fc);
			return;
		}

		e->valid = 1;
	}


	uint32_t fg = fc->color;
	for (uint32_t i = 0; i < e->count; i++)
	{
		const t_glyph_pixel *p = &e->pixels[i];
		blend_font_pixel(fc->display, fg, x + p->x, baseline + p->y, p->coverage);
	}
}

void Display::draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font *font)
{
	t_font_context font_context;

	if (IS_TRANSPARENT(color) || !text)
		return;

	if (!font)
		font = &main_font;

	if (!font || !font->data || !font->units_per_em || h <= 0)
		return;

	int cursor_x = x;
	int cursor_y = y;
	int line_height = h;

	int baseline = y + ((int)font->ascender * h) / (font->ascender - font->descender);
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

			continue;
		}

		uint8_t codepoint = (uint8_t)text[i];
		uint16_t glyph = ttf_glyph_for_codepoint(font, codepoint);
		draw_cached_glyph(&font_context, font, glyph, cursor_x, baseline, h);
		cursor_x += (ttf_glyph_advance(font, glyph) * h) / font->units_per_em;
	}
}


static int text_width_for_size(const char *text, int font_size, t_font *font)
{
	if (!text || !font || !font->data || !font->units_per_em || font_size <= 0)
		return 0;

	int line_width = 0;
	int max_width = 0;

	for (uint32_t i = 0; text[i]; i++)
	{
		if (text[i] == '\n')
		{
			if (line_width > max_width)
				max_width = line_width;

			line_width = 0;
			continue;
		}

		uint16_t glyph = ttf_glyph_for_codepoint(font, (uint8_t)text[i]);
		line_width += (ttf_glyph_advance(font, glyph) * font_size) / font->units_per_em;
	}

	if (line_width > max_width)
		max_width = line_width;

	return max_width;
}

int Display::text_width(int font_size, const char *text, t_font *font)
{
	if (!font)
		font = &main_font;

	return text_width_for_size(text, font_size, font);
}


void	Display::draw_text_at(int x, int y, int font_size, const char *text, uint32_t color, t_font *font)
{
	if (IS_TRANSPARENT(color))
		return;

	if (!font)
		font = &main_font;

	int width = text_width_for_size(text, font_size, font);
	if (width <= 0)
		return;

	draw_text(x, y, width, font_size, text, color, font);
}
