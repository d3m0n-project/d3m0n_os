#include "sys.h"

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
		print("Could not aquire framebuffer address!\n");
		return 1;
	}
	for (int i=0; i<55; i++)
	{
		d.putpixel(i, 5, 0xffffffff);
	}
	return 0;
}