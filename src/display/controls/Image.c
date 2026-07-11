#include "controls_graphics.h"
#include "icons.h"

void	ctrl_draw_image(t_control	*control)
{
	BmpTexture	texture;
	BmpTexture	*tmp_icon = 0;

	for (int i=0; control->image[i]; i++)
	{
		if (control->image[i] == '/')
		{
			tmp_icon = &texture;
			if (bmp_load_image(tmp_icon, control->image))
			{
				log("Could not load image '%s' for control '%s'\n", LOG_ERROR, control->image, control->name);
				return;
			}
			break;
		}
	}

	if (!tmp_icon) // if string could be a valid icon name
	{
		tmp_icon = get_icon(control->image, get_config()); // still not found an icon
		if (!tmp_icon)
			return;
	}
		
	draw_bmp(control->p_client_location.x, control->p_client_location.y, control->width, control->height, tmp_icon, control->color);
	
	if (tmp_icon == &texture) // if not an icon
		free_bmp_texture(tmp_icon);
	// TODO: optimize img loading
}