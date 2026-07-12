#include "display.h"
#include "log.h"

#if DEBUG == 1
#include "mailbox.h"
static volatile uint32_t	g_framebuffer_mbox[35]  __attribute__((aligned(16)));
#else
#include "lcd.h"
#endif

static t_font				main_font;

uint32_t	DISPLAY_COLORS[17] = { 0x00000000, 0xff000000, 0xff0000bf, 0xff0000ff, 0xff007fff, 0xff00ffff, 0xff00bf00, 0xff00ff00, 0xffbf5f00, 0xffff0000, 0xffbfbf00, 0xffffff00, 0xffbf00bf, 0xffff00ff, 0xff191919, 0xff7f7f7f, 0xffffffff};

uint16_t	rgb888_to_rgb565(uint32_t color)
{
	uint8_t b = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t r = color & 0xFF;

	return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void	put_pixel(int x, int y, uint32_t color)
{
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
	#if DEBUG == 1
	for (int i=0; i<w; i++)
		put_pixel(x + i, y, color);
	#else
	lcd_drawrect(x, y, w, 1, rgb888_to_rgb565(color));
	#endif
}

void	draw_vline(int x, int y, int h, uint32_t color)
{
	#if DEBUG == 1
	for (int i=0; i<h; i++)
		put_pixel(x, y + i, color);
	#else
	lcd_drawrect(x, y, 1, h, rgb888_to_rgb565(color));
	#endif
}

void	draw_rect(int x, int y, int w, int h, uint32_t color)
{
	#if DEBUG == 1
	for(int j=0; j<h; j++)
		draw_hline(x, y + j, w, color);
	#else
	lcd_drawrect(x, y, w, h, rgb888_to_rgb565(color));
	#endif
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

void draw_bmp(int x, int y, int w, int h, BmpTexture *texture, uint32_t override_color)
{
	if (!texture || !texture->pixels)
		return;

	const int tex_w = texture->width;
	const int tex_h = texture->height;

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

void	draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font	*font)
{
	uint32_t	i;
	int		bytes_per_line;
	int		src_w;
	int		src_h;
	int		dst_w;
	int		dst_h;
	int		cursor_x;
	int		cursor_y;
	int		dx;
	int		dy;
	int		src_x;
	int		src_y;
	int		row_offset;
	uint16_t	data;
	char		c;

	if (!text)
		return;
	if (!font)
		font = &main_font;
	if (!font || !font->data || font->dot_count <= 0)
		return;

	bytes_per_line = (int)(font->dot_count / 16);
	if (bytes_per_line <= 0)
		return;

	src_w = 8 * bytes_per_line;
	src_h = font->dot_count;
	dst_w = (w > 0) ? w : src_w;
	dst_h = (h > 0) ? h : src_h;
	if (dst_w <= 0 || dst_h <= 0)
		return;

	cursor_x = x;
	cursor_y = y;
	i = 0;
	while (text[i])
	{
		c = text[i];
		if (c == '\n')
		{
			cursor_x = x;
			cursor_y += dst_h;
			i++;
			continue;
		}

		for (dy = 0; dy < dst_h; dy++)
		{
			src_y = (dy * src_h) / dst_h;
			row_offset = 17 + ((int)(uint8_t)c * src_h + src_y) * bytes_per_line;
			data = font->data[row_offset];
			if (bytes_per_line == 2)
			{
				data <<= 8;
				data |= font->data[row_offset + 1];
			}

			for (dx = 0; dx < dst_w; dx++)
			{
				src_x = (dx * src_w) / dst_w;
				if (data & (1U << ((8 * bytes_per_line - 1) - src_x)))
					put_pixel(cursor_x + dx, cursor_y + dy, color);
			}
		}
		cursor_x += dst_w;
		i++;
	}
}

int display_init()
{
	if (load_font("fonts/ILGH32XF.FNT", &main_font, 32)) // TODO: change font, maybe load vector one
		return 1;
	#if DEBUG == 1
	if(!framebuffer_init(640, 480, 32))
	{
		log("Framebuffer allocated!\n", LOG_SUCCESS);
		// draw screen outline
		draw_qemu_outline();
		draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00000000);
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