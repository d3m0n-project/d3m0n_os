#include "controls_graphics.h"
#include "icons.h"

void	ctrl_draw_image(t_control *control)
{
	BmpTexture		*tmp_icon;
	t_window		*window;
	t_bmp_cache		*cache;
	t_bmp_cache		*last;

	tmp_icon = 0;
	window = get_current_window();

	if (control->image[0] == '\0') // if no image, skip
		return;

	// if its a path load it
	if (ft_strchr(control->image, '/'))
	{
		cache = window->bmp_cache;
		last = 0;

		// search cache
		while (cache)
		{
			if (!ft_strcmp(cache->path, control->image))
			{
				tmp_icon = cache->texture;
				break;
			}
			last = cache;
			cache = cache->next;
		}

		// cache it
		if (!tmp_icon)
		{
			tmp_icon = ft_calloc(1, sizeof(BmpTexture));
			if (!tmp_icon)
				return;

			if (bmp_load_image(tmp_icon, control->image))
			{
				log("Could not load image '%s' for control '%s'\n", LOG_ERROR, control->image, control->name);
				free(tmp_icon);
				return;
			}

			cache = ft_calloc(1, sizeof(t_bmp_cache));
			if (!cache)
			{
				free_bmp_texture(tmp_icon);
				free(tmp_icon);
				return;
			}

			ft_strlcpy(cache->path, control->image, sizeof(cache->path));
			cache->texture = tmp_icon;

			if (!window->bmp_cache)
				window->bmp_cache = cache;
			else
				last->next = cache;
		}
	}
	else
	{
		// icon
		tmp_icon = get_icon(control->image, get_config());
		if (!tmp_icon)
			return;
	}

	draw_bmp(control->p_client_location.x, control->p_client_location.y, control->p_client_size.x, control->p_client_size.y, tmp_icon, control->color);
}