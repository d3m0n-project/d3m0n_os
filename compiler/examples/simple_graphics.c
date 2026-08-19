#include "sys.h"

static inline void	putpixel(int x, int y, uint32_t color)
{
	if ((color & 0xFF000000) == 0x00000000) // TODO: real transparency
		return;
	volatile uint32_t *fb = (volatile uint32_t *)(uintptr_t)fb_req.fb_addr;
	fb[y * (fb_req.pitch / 4) + x] = color;
}

int	app_main(void)
{
	uint8_t	*fb;
	int		width = 0;
	int		height = 0;

	getfbaddr(&fb, &width, &height);

	return 0;
}