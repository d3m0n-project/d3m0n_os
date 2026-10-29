#include "display/display.h"
#include "log.h"
#include "filesystem/filesystem.h"
#include "memory/memory.h"
#include "memory/mmu.h"
#define malloc kmalloc
#define free kfree
#include "../../compiler/sdk/lib/app/parsing/png.h"
#undef malloc
#undef free

#if DEBUG == 1
#include "protocol/mailbox.h"
static volatile uint32_t	g_framebuffer_mbox[35]  __attribute__((aligned(16)));
#else
#include "display/lcd.h"
#endif

//static t_font				main_font;

uint32_t	DISPLAY_COLORS[17] = { 0x00000000, 0xff000000, 0xff0000bf, 0xff0000ff, 0xff007fff, 0xff00ffff, 0xff00bf00, 0xff00ff00, 0xffbf5f00, 0xffff0000, 0xffbfbf00, 0xffffff00, 0xffbf00bf, 0xffff00ff, 0xff191919, 0xff7f7f7f, 0xffffffff};

static uint32_t	*g_draw_target;
static int		g_draw_target_width;
static int		g_draw_target_height;

void	display_set_draw_target(uint32_t *pixels, int width, int height)
{
	g_draw_target = pixels;
	g_draw_target_width = width;
	g_draw_target_height = height;
}

void	display_clear_draw_target(void)
{
	g_draw_target = 0;
	g_draw_target_width = 0;
	g_draw_target_height = 0;
}

uintptr_t	get_fb_addr(int *pitch)
{
	*pitch = 0;
#if DEBUG == 1
	*pitch = fb_req.pitch;
	return fb_req.fb_addr;
#else
	return 0;
#endif
}

uint16_t	rgb888_to_rgb565(uint32_t color)
{
	uint8_t b = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t r = color & 0xFF;

	return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void	put_pixel(int x, int y, uint32_t color)
{
	if (g_draw_target)
	{
		if (x < 0 || x >= g_draw_target_width || y < 0 || y >= g_draw_target_height)
			return;
		if ((color & 0xFF000000) == 0x00000000)
			return;
		g_draw_target[y * g_draw_target_width + x] = color;
		return;
	}
	if (x < 0 || (uint32_t)x >= SCREEN_WIDTH || y < 0 || (uint32_t)y >= SCREEN_HEIGHT)
		return;
	if ((color & 0xFF000000) == 0x00000000) // TODO: real transparency
		return;
	#if DEBUG == 1
	volatile uint32_t *fb = (volatile uint32_t *)(uintptr_t)fb_req.fb_addr;
	fb[y * (fb_req.pitch / 4) + x] = color;
	#else
	lcd_drawrect(x, y, 1, 1, rgb888_to_rgb565(color));
	#endif
}

uint32_t	get_pixel(int x, int y)
{
	if (g_draw_target)
	{
		if (x < 0 || x >= g_draw_target_width || y < 0 || y >= g_draw_target_height)
			return 0;
		return g_draw_target[y * g_draw_target_width + x];
	}
	if (x < 0 || (uint32_t)x >= SCREEN_WIDTH || y < 0 || (uint32_t)y >= SCREEN_HEIGHT)
		return 0;
	#if DEBUG == 1
	volatile uint32_t *fb = (volatile uint32_t *)(uintptr_t)fb_req.fb_addr;
	return fb[y * (fb_req.pitch / 4) + x];
	#endif
	return 0; // TODO: real lcd get pixel
}

#if DEBUG == 1
void	draw_qemu_outline()
{
	volatile uint32_t *fb = (volatile uint32_t *)(uintptr_t)fb_req.fb_addr;
	// vertical
	for (int y=0; y<SCREEN_HEIGHT + 1; y++)
		fb[y * (fb_req.pitch / 4) + SCREEN_WIDTH] = OUTLINE_COLOR;
}
#endif

void	draw_hline(int x, int y, int w, uint32_t color)
{
	if (w < 0)
	{
		w = -w;
		x -= w;
	}
	if (g_draw_target)
	{
		for (int i = 0; i < w; i++)
			put_pixel(x + i, y, color);
		return;
	}
	#if DEBUG == 1
	for (int i=0; i<w; i++)
		put_pixel(x + i, y, color);
	#else
	lcd_drawrect(x, y, w, 1, rgb888_to_rgb565(color));
	#endif
}

void	draw_vline(int x, int y, int h, uint32_t color)
{
	if (g_draw_target)
	{
		for (int i = 0; i < h; i++)
			put_pixel(x, y + i, color);
		return;
	}
	#if DEBUG == 1
	for (int i=0; i<h; i++)
		put_pixel(x, y + i, color);
	#else
	lcd_drawrect(x, y, 1, h, rgb888_to_rgb565(color));
	#endif
}

void	draw_rect(int x, int y, int w, int h, uint32_t color)
{
	if (g_draw_target)
	{
		for (int j = 0; j < h; j++)
			draw_hline(x, y + j, w, color);
		return;
	}
	#if DEBUG == 1
	for(int j=0; j<h; j++)
		draw_hline(x, y + j, w, color);
	#else
	lcd_drawrect(x, y, w, h, rgb888_to_rgb565(color));
	#endif
}

void	draw_rounded_rect(int x, int y, int w, int h, int radius, uint32_t color)
{
	if (w <= 0 || h <= 0)
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
				put_pixel(px, py, color);
				continue;
			}

			int dx = px - cx;
			int dy = py - cy;

			if (dx * dx + dy * dy <= radius * radius)
				put_pixel(px, py, color);
		}
	}
}

void	draw_rect_outline(int x, int y, int w, int h, uint32_t color)
{
	draw_hline(x, y, w, color);
	draw_hline(x, y + h, w, color);
	draw_vline(x, y, h, color);
	draw_vline(x + w, y, h, color);
}

static inline uint32_t	alpha_blend(uint32_t src, uint32_t dst)
{
	uint32_t a = src >> 24;

	if (a == 255)
		return src;
	if (a == 0)
		return dst;

	uint32_t sr = (src >> 16) & 255;
	uint32_t sg = (src >> 8) & 255;
	uint32_t sb = src & 255;

	uint32_t dr = (dst >> 16) & 255;
	uint32_t dg = (dst >> 8) & 255;
	uint32_t db = dst & 255;

	uint32_t inv = 255 - a;

	uint32_t r = (sr * a + dr * inv) / 255;
	uint32_t g = (sg * a + dg * inv) / 255;
	uint32_t b = (sb * a + db * inv) / 255;

	return 0xFF000000 | (r << 16) | (g << 8) | b;
}

static void	draw_ellipse_points(int cx, int cy, int x, int y, uint32_t color)
{
	put_pixel(cx + x, cy + y, color);
	put_pixel(cx - x, cy + y, color);
	put_pixel(cx + x, cy - y, color);
	put_pixel(cx - x, cy - y, color);
}

void	draw_ellipse(int cx, int cy, int rx, int ry, uint32_t color, int filled)
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
			draw_hline(cx - x, cy + y, 2 * x + 1, color);
			draw_hline(cx - x, cy - y, 2 * x + 1, color);

			if (x != y)
			{
				draw_hline(cx - y, cy + x, 2 * y + 1, color);
				draw_hline(cx - y, cy - x, 2 * y + 1, color);
			}
		}
		else
			draw_ellipse_points(cx, cy, x, y, color);

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
			draw_hline(cx - x, cy + y, 2 * x + 1, color);
			draw_hline(cx - x, cy - y, 2 * x + 1, color);

			if (x != y)
			{
				draw_hline(cx - y, cy + x, 2 * y + 1, color);
				draw_hline(cx - y, cy - x, 2 * y + 1, color);
			}
		}
		else
			draw_ellipse_points(cx, cy, x, y, color);

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

void	draw_bmp(int x, int y, int w, int h, BmpTexture *texture, uint32_t override_color)
{
	if (!texture || !texture->pixels)
		return;

	if (w <= 0 || h <= 0)
		return;

	const int	tex_w = texture->width;
	const int	tex_h = texture->height;

	// faster lcd
	#if DEBUG == 0
	if (w == tex_w && h == tex_h)
	{
		//if (((override_color >> 24) & 0xFF) == 0xFF)
		goto generic;
		lcd_blit_argb32_to_rgb565(x, y, w, h, texture->pixels);
		return;
	}

	generic:
	#endif
	{
		const float scale_x = (float)tex_w / w;
		const float scale_y = (float)tex_h / h;

		const int has_alpha = (texture->bytes_per_pixel == 4);
		const int use_override = ((override_color >> 24) == 0xFF);

		for (int j = 0; j < h; j++)
		{
			int sy = (int)(j * scale_y);
			if (sy >= tex_h)
				sy = tex_h - 1;

			const uint32_t *src_row = texture->pixels + sy * tex_w;

			for (int i = 0; i < w; i++)
			{
				int sx = (int)(i * scale_x);
				if (sx >= tex_w)
					sx = tex_w - 1;

				uint32_t color = src_row[sx];
				if (has_alpha)
				{
					uint32_t alpha = color >> 24;
					if (alpha == 0)
						continue;
				}

				if (use_override)
					color = (color & 0xFF000000) | (override_color & 0x00FFFFFF);

				if (has_alpha && (color >> 24) != 255)
				{
					uint32_t dst = get_pixel(x + i, y + j);
					color = alpha_blend(color, dst);
				}
				put_pixel(x + i, y + j, color);
			}
		}
	}
}

int	draw_png(int x, int y, int w, int h, const char *path)
{
	if (!path || w <= 0 || h <= 0)
		return 1;
	int fd = open(path, O_READ);
	if (fd < 0)
		return 1;
	uint32_t file_size = lseek(fd, 0, SEEK_END);
	if (!file_size || file_size > 8388608 || lseek(fd, 0, SEEK_SET) == (uint32_t)-1)
	{
		close(fd);
		return 1;
	}
	unsigned char *data = (unsigned char *)kmalloc(file_size);
	if (!data)
	{
		close(fd);
		return 1;
	}
	uint32_t offset = 0;
	while (offset < file_size)
	{
		uint32_t amount = read(fd, (char *)data + offset, file_size - offset);
		if (!amount || amount == (uint32_t)-1)
		{
			kfree(data);
			close(fd);
			return 1;
		}
		offset += amount;
	}
	close(fd);
	uint32_t image_width, image_height, *pixels;
	if (!png_decode(data, file_size, &image_width, &image_height, &pixels))
	{
		kfree(data);
		return 1;
	}
	kfree(data);
	for (int py = 0; py < h; py++)
	{
		uint32_t source_y = (uint32_t)((uint64_t)py * image_height / h);
		for (int px = 0; px < w; px++)
		{
			uint32_t source_x = (uint32_t)((uint64_t)px * image_width / w);
			uint32_t color = pixels[source_y * image_width + source_x];
			uint32_t alpha = color >> 24;
			if (!alpha)
				continue;
			if (alpha < 255)
			{
				uint32_t background = get_pixel(x + px, y + py), blended = 0xff000000;
				for (uint32_t shift = 0; shift < 24; shift += 8)
				{
					uint32_t foreground_channel = (color >> shift) & 0xff;
					uint32_t background_channel = (background >> shift) & 0xff;
					blended |= ((foreground_channel * alpha + background_channel * (255 - alpha)) / 255) << shift;
				}
				color = blended;
			}
			put_pixel(x + px, y + py, color);
		}
	}
	kfree(pixels);
	return 0;
}


#if DEBUG == 1
int framebuffer_init(uint32_t width, uint32_t height, uint32_t bpp)
{
	g_framebuffer_mbox[0] = sizeof(g_framebuffer_mbox);
	g_framebuffer_mbox[1] = MAILBOX_REQUEST;
	g_framebuffer_mbox[2] = MAILBOX_TAG_SET_PHYS_SIZE;
	g_framebuffer_mbox[3] = 8;
	g_framebuffer_mbox[4] = 8;
	g_framebuffer_mbox[5] = width;
	g_framebuffer_mbox[6] = height;
	g_framebuffer_mbox[7] = MAILBOX_TAG_SET_VIRT_SIZE;
	g_framebuffer_mbox[8] = 8;
	g_framebuffer_mbox[9] = 8;
	g_framebuffer_mbox[10] = width;
	g_framebuffer_mbox[11] = height;
	g_framebuffer_mbox[12] = MAILBOX_TAG_SET_VIRT_OFFSET;
	g_framebuffer_mbox[13] = 8;
	g_framebuffer_mbox[14] = 8;
	g_framebuffer_mbox[15] = 0;
	g_framebuffer_mbox[16] = 0;
	g_framebuffer_mbox[17] = MAILBOX_TAG_SET_DEPTH;
	g_framebuffer_mbox[18] = 4;
	g_framebuffer_mbox[19] = 4;
	g_framebuffer_mbox[20] = bpp;
	g_framebuffer_mbox[21] = MAILBOX_TAG_SET_PIXEL_ORDER;
	g_framebuffer_mbox[22] = 4;
	g_framebuffer_mbox[23] = 4;
	g_framebuffer_mbox[24] = 1;
	g_framebuffer_mbox[25] = MAILBOX_TAG_ALLOCATE_BUFFER;
	g_framebuffer_mbox[26] = 8;
	g_framebuffer_mbox[27] = 8;
	g_framebuffer_mbox[28] = 16;
	g_framebuffer_mbox[29] = 0;
	g_framebuffer_mbox[30] = MAILBOX_TAG_GET_PITCH;
	g_framebuffer_mbox[31] = 4;
	g_framebuffer_mbox[32] = 4;
	g_framebuffer_mbox[33] = 0;
	g_framebuffer_mbox[34] = MAILBOX_TAG_LAST;
	if (!mailbox_call(g_framebuffer_mbox, MAILBOX_CH_PROP))
		return -1;
	if (g_framebuffer_mbox[1] != MAILBOX_RESPONSE_SUCCESS || g_framebuffer_mbox[28] == 0 || g_framebuffer_mbox[33] == 0)
		return -1;
	fb_req.width = g_framebuffer_mbox[5];
	fb_req.height = g_framebuffer_mbox[6];
	fb_req.virt_width = g_framebuffer_mbox[10];
	fb_req.virt_height = g_framebuffer_mbox[11];
	fb_req.x_offset = g_framebuffer_mbox[15];
	fb_req.y_offset = g_framebuffer_mbox[16];
	fb_req.depth = g_framebuffer_mbox[20];
	fb_req.pitch = g_framebuffer_mbox[33];
	fb_req.fb_addr = (uintptr_t)(g_framebuffer_mbox[28] & 0x3FFFFFFF);
	fb_req.fb_size = g_framebuffer_mbox[29];
	return 0;
}
#endif

static void	font_pixel(void *context, int x, int y, uint8_t coverage)
{
	uint32_t bg = get_pixel(x, y);
	uint32_t fg = (uint32_t)(uintptr_t)context;
	uint32_t r = (((fg >> 16) & 0xff) * coverage
		+ ((bg >> 16) & 0xff) * (255 - coverage)) / 255;
	uint32_t g = (((fg >> 8) & 0xff) * coverage
		+ ((bg >> 8) & 0xff) * (255 - coverage)) / 255;
	uint32_t b = ((fg & 0xff) * coverage
		+ (bg & 0xff) * (255 - coverage)) / 255;

	put_pixel(x, y, 0xFF000000 | (r << 16) | (g << 8) | b);
}

void	draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font	*font)
{
	if (!text)
		return;
	if (!font)
	{
		log("DRAW TEXT: No font provided, could not write\n", LOG_ERROR);
		return;
	}
	if (!font->data || !font->units_per_em || h <= 0)
		return;
	int cursor_x = x;
	int cursor_y = y;
	int line_height = h;
	int baseline = y + ((int)font->ascender * h) /
		(font->ascender - font->descender);
	(void)w;
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
			(void *)(uintptr_t)color);
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
		line_width += (ttf_glyph_advance(font, glyph) * font_size)
			/ font->units_per_em;
	}
	if (line_width > max_width)
		max_width = line_width;
	return max_width;
}

void	draw_text_at(int x, int y, int font_size, const char *text, uint32_t color, t_font *font)
{
	int	width;

	width = text_width_for_size(text, font_size, font);
	if (width <= 0)
		return;
	draw_text(x, y, width, font_size, text, color, font);
}

int display_init()
{
	#if DEBUG == 1
	if(!framebuffer_init(640, 480, 32))
	{
		log("Framebuffer allocated!\n", LOG_SUCCESS);
		// draw screen outline
		draw_qemu_outline();
		draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00000000);
		mmu_clean_invalidate_dcache_range((uint32_t)fb_req.fb_addr, fb_req.fb_size);
		return 0;
	} else {
		log("Framebuffer allocation failed!\n", LOG_ERROR);
		return 1;
	}
	#else
	if(!lcd_init())
	{
		log("LCD Display initialized successfully!\n", LOG_SUCCESS);
		lcd_clear(rgb888_to_rgb565(DISPLAY_COLORS[WHITE]));
		return 0;
	}
	else
	{
		log("Display initialization failed!\n", LOG_ERROR);
		return 1;
	}
	#endif
}
