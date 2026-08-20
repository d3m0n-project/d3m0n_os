#include "sys.h"
#include "stdlib.h"
#include "stdio.hpp"

class Display
{
private:
	uint8_t	*fb;
	int		pitch;
public:
	int		w = 0;
	int		h = 0;
	Display()
	{
		getfbaddr(&this->fb, &this->w, &this->h, &this->pitch);
		printf("created a w=%i h=%i pitch=%i\n", this->w, this->h, this->pitch);
	}

	void	putpixel(int x, int y, uint32_t color)
	{
		if ((color & 0xFF000000) == 0x00000000) // TODO: real transparency
			return;
		volatile uint32_t *framebuffer = (volatile uint32_t *)this->fb;
		framebuffer[y * (this->pitch / 4) + x] = color;
	}
};


extern "C" int	app_main(void)
{
	Display	d = Display();
	if (d.w == 0 || d.h == 0)
	{
		printf("Could not aquire framebuffer address!\n");
		return 1;
	}
	for (int y=0; y<=d.h; y++)
	{
		for (int x=0; x<=d.w; x++)
			d.putpixel(x, y, 0xffffffff);
	}
	return 0;
}