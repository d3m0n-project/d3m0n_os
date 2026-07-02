#include "controls_graphics.h"

void	ctrl_draw_image(t_control	*control)
{
	BmpTexture	texture;
	if (bmp_load_image(&texture, control->image))
	{
		log("Could not load image '%s' for control '%s'\n", LOG_ERROR | LOG_INDENT, control->image, control->name);
		return;
	}
	draw_bmp(control->location.x, control->location.y, control->width, control->height, &texture);
	free_bmp_texture(&texture);
}