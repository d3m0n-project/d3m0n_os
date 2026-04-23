#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "bmp.h"
#include "color.h"

#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	320

typedef struct s_font
{
	int		dot_count;
	char	name[25];
	uint8_t	*data;
}	t_font;

extern uint32_t			DISPLAY_COLORS[16];

int		display_init();
void	put_pixel(int x, int y, uint32_t color);
void	draw_rect(int x, int y, int w, int h, uint32_t color);
void	draw_bmp(int x, int y, int w, int h, BmpTexture *texture);
int		framebuffer_init(uint32_t width, uint32_t height, uint32_t bpp);

void	display_log_click(int x, int y);
void	display_click_demo(void);
void	display_track_ps2_mouse(int8_t dx, int8_t dy);


void	draw_text(int x, int y, int w, int h, const char *text, uint32_t color, t_font	*font);
int		load_font(const char *path, t_font	*out, int dot_count);
void	free_font(t_font *font);

#endif