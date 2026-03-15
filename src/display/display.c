#include "display.h"
#include "mailbox.h"
#include "log.h"

static volatile uint32_t	g_framebuffer_mbox[35] __attribute__((aligned(16)));

void	put_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || (uint32_t)x >= SCREEN_WIDTH || y < 0 || (uint32_t)y >= SCREEN_HEIGHT)
		return;
	volatile uint32_t *fb = (volatile uint32_t *)fb_req.fb_addr;
    fb[y * (fb_req.pitch / 4) + x] = color;
}

void	draw_hline(int x, int y, int w, uint32_t color)
{
	//memset()
	for (int i=0; i<w; i++)
		put_pixel(x + i, y, color);
}

void	draw_rect(int x, int y, int w, int h, uint32_t color)
{
	for(int j=0; j<h; j++)
		draw_hline(x, y + j, w, color);
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

int	display_init()
{
	#if DEBUG == 1
	if(!framebuffer_init(640, 480, 32))
	{
		log("Framebuffer allocated!\n", LOG_SUCCESS);
		draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00FFFFFF);
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