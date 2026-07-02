#include "controls_graphics.h"

void	ctrl_draw_button(t_control *control)
{
	int	inner_x, inner_y, inner_w, inner_h;
	int	char_w, char_h, text_w, text_h;
	int	draw_x, draw_y;

	int font_size = control_text_auto_font_size(control);
	char_w = font_size;
	char_h = font_size * 2;
	compute_inner_rect(control, &inner_x, &inner_y, &inner_w, &inner_h);
	// background
	if (control->bg_color)
		draw_rect(inner_x, inner_y, inner_w, inner_h, control->bg_color);
	// image (optional)
	if (control->image[0])
	{
		BmpTexture texture;
		if (!bmp_load_image(&texture, control->image) && texture.pixels)
		{
			int dst_w = inner_w;
			int dst_h = inner_h;
			int dst_x = inner_x;
			int dst_y = inner_y;
			if (control->mode == 2) // AUTO_SIZE
			{
				dst_w = texture.width;
				dst_h = texture.height;
			}
			else if (control->mode == 1) // ZOOM
			{
				float sx = (float)inner_w / (float)texture.width;
				float sy = (float)inner_h / (float)texture.height;
				float s = (sx < sy) ? sx : sy;
				dst_w = (int)(texture.width * s);
				dst_h = (int)(texture.height * s);
				dst_x = inner_x + (inner_w - dst_w) / 2;
				dst_y = inner_y + (inner_h - dst_h) / 2;
			}
			else if (control->mode == 3) // CENTER
			{
				dst_w = texture.width;
				dst_h = texture.height;
				dst_x = inner_x + (inner_w - dst_w) / 2;
				dst_y = inner_y + (inner_h - dst_h) / 2;
			}
			draw_bmp(dst_x, dst_y, dst_w, dst_h, &texture);
			free_bmp_texture(&texture);
		}
	}
	// text
	control_text_metrics(control->content, &text_w, &text_h);
	// control_text_metrics returns char sizes in pixel
	text_w = char_w * text_w;
	text_h = char_h * (text_h > 0 ? text_h : 1);
	compute_text_position(control->text_align, inner_x, inner_y, inner_w, inner_h, text_w, text_h, &draw_x, &draw_y);
	draw_text(draw_x, draw_y, char_w, char_h, control->content, control->color, 0);
}