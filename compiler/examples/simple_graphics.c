#include "sys.h"

int	app_main(void)
{
	uint8_t	*fb;
	int		width = 0;
	int		height = 0;

	getfbaddr(&fb, &width, &height);

	if (width == 320 && height == 480)
		print("valid\n");
	else
		print("not valid\n");

	return 0;
}