#include "display.h"
#include "mailbox.h"
#include "log.h"

static volatile uint32_t	g_framebuffer_mbox[35] __attribute__((aligned(16)));
static t_font				main_font;

static int	display_clamp_i32(int value, int min, int max)
{
    if (value < min)
        return (min);
    if (value > max)
        return (max);
    return (value);
}

static void	display_u32_to_str(unsigned int value, char *out)
{
    char	buffer[12];
    int		index;
    int		pos;

    if (!out)
        return;
    if (value == 0)
    {
        out[0] = '0';
        out[1] = '\0';
        return;
    }
    index = 0;
    while (value > 0 && index < (int)sizeof(buffer))
    {
        buffer[index++] = (char)('0' + (value % 10));
        value /= 10;
    }
    pos = 0;
    while (index > 0)
        out[pos++] = buffer[--index];
    out[pos] = '\0';
}

uint32_t	DISPLAY_COLORS[16] = { 0x00000000, 0x000000bf, 0x000000ff, 0x00007fff, 0x0000ffff, 0x0000bf00, 0x0000ff00, 0x00bf5f00, 0x00ff0000, 0x00bfbf00, 0x00ffff00, 0x00bf00bf, 0x00ff00ff, 0x00191919, 0x007f7f7f, 0x00ffffff};

void	display_log_click(int x, int y)
{
    char x_buf[12];
    char y_buf[12];

    log("Screen click at x=%i y=%i\n", LOG_INFO, x, y);
    draw_rect(0, 0, SCREEN_WIDTH, 20, DISPLAY_COLORS[BLACK]);
    display_u32_to_str((unsigned int)x, x_buf);
    display_u32_to_str((unsigned int)y, y_buf);
    draw_text(2, 2, 16, 16, "x", DISPLAY_COLORS[WHITE], 0);
    draw_text(10, 2, 48, 16, x_buf, DISPLAY_COLORS[WHITE], 0);
    draw_text(62, 2, 16, 16, "y", DISPLAY_COLORS[WHITE], 0);
    draw_text(70, 2, 48, 16, y_buf, DISPLAY_COLORS[WHITE], 0);
}

void	display_click_demo(void)
{
    log("Click demo is available, but no kernel touch backend is wired yet.\n", LOG_WARNING);
}

void	display_track_ps2_mouse(int8_t dx, int8_t dy)
{
    static int	mouse_x;
    static int	mouse_y;
    static int	initialized;

    if (!initialized)
    {
        mouse_x = SCREEN_WIDTH / 2;
        mouse_y = SCREEN_HEIGHT / 2;
        initialized = 1;
    }
    mouse_x += (int)dx;
    mouse_y -= (int)dy;
    mouse_x = display_clamp_i32(mouse_x, 0, SCREEN_WIDTH - 1);
    mouse_y = display_clamp_i32(mouse_y, 0, SCREEN_HEIGHT - 1);
    log("PS/2 mouse pos x=%i y=%i\n", LOG_INFO, mouse_x, mouse_y);
}

void	put_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || (uint32_t)x >= SCREEN_WIDTH || y < 0 || (uint32_t)y >= SCREEN_HEIGHT)
		return;
	volatile uint32_t *fb = (volatile uint32_t *)fb_req.fb_addr;
    fb[y * (fb_req.pitch / 4) + x] = color;
}

void	draw_hline(int x, int y, int w, uint32_t color)
{
	for (int i=0; i<w; i++)
		put_pixel(x + i, y, color);
}

void	draw_rect(int x, int y, int w, int h, uint32_t color)
{
	for(int j=0; j<h; j++)
		draw_hline(x, y + j, w, color);
}

void draw_bmp(int x, int y, int w, int h, BmpTexture *texture)
{
    if (!texture || !texture->pixels)
        return;

    float scale_x = (float)texture->width / (float)w;
    float scale_y = (float)texture->height / (float)h;

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int src_x = (int)(i * scale_x);
            int src_y = (int)(j * scale_y);

            if (src_x >= texture->width)
				src_x = texture->width - 1;
            if (src_y >= texture->height)
				src_y = texture->height - 1;

            put_pixel(x + i, y + j, texture->pixels[src_y * texture->width + src_x]);
        }
    }
}

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
        return (-1);
    if (g_framebuffer_mbox[1] != MAILBOX_RESPONSE_SUCCESS
        || g_framebuffer_mbox[28] == 0
        || g_framebuffer_mbox[33] == 0)
        return (-1);
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
    return (0);
}

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

int	display_init()
{
	if (load_font("fonts/ILGH32XF.FNT", &main_font, 32))
		return 1;
	#if DEBUG == 1
	if(!framebuffer_init(640, 480, 32))
	{
		log("Framebuffer allocated!\n", LOG_SUCCESS);
		draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00000000);
		return 0;
	} else {
		log("Framebuffer allocation failed!\n", LOG_ERROR);
		return 1;
	}
	#else
	log("REAL mode display not yet supported!\n", LOG_ERROR);
	return 1;
	#endif
}