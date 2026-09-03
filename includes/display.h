#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "bmp.h"
#include "color.h"
#include "ttf.h"

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		480

#define TOPBAR_HEIGHT		32
#define TOPBAR_PADDING		7
#define TOPBAR_FONT_SIZE	14

#define OUTLINE_COLOR		0xFFFF00FF


#define LIGHT_THEME_FG		0xFF202124
#define LIGHT_THEME_BG		0xFFFFFFFF

#define DARK_THEME_FG		0xFFFFFFFF
#define DARK_THEME_BG		0xFF121212



#ifndef DEBUG_OUTLINE
	#define DEBUG_OUTLINE	0
#endif

#define CURSOR_SIZE			4

extern uint32_t			DISPLAY_COLORS[17];

int					display_init();
uint32_t			get_pixel(int x, int y);
void				put_pixel(int x, int y, uint32_t color);
void				draw_rect(int x, int y, int w, int h, uint32_t color);
void				draw_ellipse(int cx, int cy, int rx, int ry, uint32_t color, int filled);
void				draw_rect_outline(int x, int y, int w, int h, uint32_t color);
void				draw_bmp(int x, int y, int w, int h, BmpTexture *texture, uint32_t override_color);
uintptr_t			get_fb_addr(int *pitch);

void				draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font	*font);
void				draw_text_at(int x, int y, int font_size, const char *text, uint32_t color, t_font *font);
int					load_font(const char *path, t_font	*out);
void				free_font(t_font *font);

#endif
