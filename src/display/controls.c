#include "controls.h"
#include "display.h"
#include "log.h"
#include "memory.h"
#include "libft.h"

static void	compute_text_position(int anchor, int box_x, int box_y, int box_w, int box_h,
				int text_w, int text_h, int *out_x, int *out_y)
{
	int tx = box_x;
	int ty = box_y;
	if (anchor & ANCHOR_LEFT)
		tx = box_x;
	else if (anchor & ANCHOR_RIGHT)
		tx = box_x + box_w - text_w;
	else
		tx = box_x + (box_w - text_w) / 2;

	if (anchor & ANCHOR_TOP)
		ty = box_y;
	else if (anchor & ANCHOR_BOTTOM)
		ty = box_y + box_h - text_h;
	else
		ty = box_y + (box_h - text_h) / 2;

	if (tx < box_x) tx = box_x;
	if (ty < box_y) ty = box_y;
	*out_x = tx;
	*out_y = ty;
}

static void	compute_inner_rect(const t_control *control, int *out_x, int *out_y, int *out_w, int *out_h)
{
	int x = control->location.x + control->margin_left;
	int y = control->location.y + control->margin_top;
	int w = control->width - control->margin_left - control->margin_right;
	int h = control->height - control->margin_top - control->margin_bottom;
	if (w < 0) w = 0;
	if (h < 0) h = 0;
	*out_x = x;
	*out_y = y;
	*out_w = w;
	*out_h = h;
}

static void	control_text_metrics(const char *text, int *out_max_len, int *out_lines)
{
	int i = 0;
	int cur = 0;
	int max = 0;
	int lines = 1;

	if (!text)
	{
		if (out_max_len) *out_max_len = 0;
		if (out_lines) *out_lines = 0;
		return;
	}
	while (text[i])
	{
		if (text[i] == '\n')
		{
			if (cur > max) max = cur;
			cur = 0;
			lines++;
		}
		else
			cur++;
		i++;
	}
	if (cur > max) max = cur;
	if (out_max_len) *out_max_len = max;
	if (out_lines) *out_lines = lines;
}

static int	control_text_auto_font_size(const t_control *control)
{
	int	len;
	int	from_width;
	int	from_height;
	int	size;

	if (!control)
		return (8);
	if (control->font_size > 0)
		return (control->font_size);
	len = 0;
	if (control && control->content)
	{
		int lines = 0;
		control_text_metrics(control->content, &len, &lines);
	}
	from_width = 0;
	from_height = 0;
	if (control->width > 0 && len > 0)
		from_width = control->width / len;
	if (control->height > 0)
		from_height = control->height / 2;
	if (from_width > 0 && from_height > 0)
		size = (from_width < from_height) ? from_width : from_height;
	else if (from_width > 0)
		size = from_width;
	else if (from_height > 0)
		size = from_height;
	else
		size = 8;
	if (size < 1)
		size = 1;
	return (size);
}

int	create_window(t_window *out, const char *title, int w, int h)
{
	int	i = 0;
	log("Creating a %ix%i Window...\n", LOG_INFO, w, h);
	if (!title)
	{
		log("Window title cant be null\n", LOG_ERROR | LOG_INDENT);
		return 1;
	}
	if (w < 0 || h < 0 || w > SCREEN_WIDTH || h > SCREEN_HEIGHT)
	{
		log("Maximum Window size is %ix%i, got %ix%i\n", LOG_ERROR | LOG_INDENT, SCREEN_WIDTH, SCREEN_HEIGHT, w, h);
		return 1;
	}
	while (title[i] && i < 24)
	{
		out->title[i] = ((char *)title)[i];
		i++;
	}
	if (title[i])
	{
		log("Invalid given window title length: max 24 chars\n", LOG_ERROR | LOG_INDENT);
		return 1;
	}
	out->title[i] = '\0';
	out->width = w;
	out->height = h;
	out->top_bar = 1;
	out->controls = 0;
	ft_memset(out->events, 0, MAX_WINDOW_EVENTS * sizeof(t_event));
	return 0;
}

void	add_control(t_window *to, t_control *control)
{
	t_control	*current = to->controls;

	control->p_next = 0;
	if (current == 0)
	{
		to->controls = control;
		current = to->controls;
	}
	else
	{
		while (current)
			current = current->p_next;
		current->p_next = control;
	}
	log("Added control: '%s'\n", LOG_SUCCESS, current->name);
}

void	draw_control(t_control *control)
{
	int	font_size;

	if (control->visible == 0)
		return;
	switch (control->p_type)
	{
	case CONTROL_RECT:
		draw_rect(control->location.x, control->location.y, control->width, control->height, control->bg_color);
		break;
	case CONTROL_TEXT:
		{
			int inner_x, inner_y, inner_w, inner_h;
			int char_w, char_h, text_w, text_h;
			int draw_x, draw_y;
			int maxlen = 0;
			int lines = 0;
			font_size = control_text_auto_font_size(control);
			char_w = font_size;
			char_h = font_size * 2;
			compute_inner_rect(control, &inner_x, &inner_y, &inner_w, &inner_h);
			control_text_metrics(control->content, &maxlen, &lines);
			text_w = char_w * maxlen;
			text_h = char_h * (lines > 0 ? lines : 1);
			if (control->bg_color)
				draw_rect(inner_x, inner_y, inner_w, inner_h, control->bg_color);
			compute_text_position(control->text_align, inner_x, inner_y, inner_w, inner_h, text_w, text_h, &draw_x, &draw_y);
			draw_text(draw_x, draw_y, char_w, char_h, control->content, control->color, 0);
		}
		break;

	case CONTROL_BUTTON:
		{
			int inner_x, inner_y, inner_w, inner_h;
			int char_w, char_h, text_w, text_h;
			int draw_x, draw_y;
			font_size = control_text_auto_font_size(control);
			char_w = font_size;
			char_h = font_size * 2;
			compute_inner_rect(control, &inner_x, &inner_y, &inner_w, &inner_h);
			// background
			if (control->bg_color)
				draw_rect(inner_x, inner_y, inner_w, inner_h, control->bg_color);
			// image (optional)
			if (control->image && control->image[0])
			{
				BmpTexture texture;
				if (bmp_load_image(&texture, control->image) == 0 && texture.pixels)
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
			// control_text_metrics returns char counts; convert to pixels
			text_w = char_w * text_w;
			text_h = char_h * (text_h > 0 ? text_h : 1);
			compute_text_position(control->text_align, inner_x, inner_y, inner_w, inner_h, text_w, text_h, &draw_x, &draw_y);
			draw_text(draw_x, draw_y, char_w, char_h, control->content, control->color, 0);
		}
		break;

/* duplicate IMAGE case removed (handled above) */
	case CONTROL_IMAGE:
		BmpTexture	texture;
		if (bmp_load_image(&texture, control->image))
		{
			log("Could not load image '%s' for control '%s'\n", LOG_ERROR | LOG_INDENT, control->image, control->name);
			return;
		}
		draw_bmp(control->location.x, control->location.y, control->width, control->height, &texture);
		free_bmp_texture(&texture);
		break;
	
	default:
		log("Unknown control type: id=%i\n", LOG_WARNING, control->p_type);
		break;
	}
	#if DEBUG_OUTLINE == 1
		draw_rect_outline(control->location.x, control->location.y, control->width, control->height, OUTLINE_COLOR);
	#endif
}

void	draw_window(t_window *window)
{
	t_control	*current = window->controls;

	draw_rect(0, 0, window->width, window->height, window->bg_color);
	while (current)
	{
		draw_control(current);
		// TODO: children
		current = current->p_next;
	}
	if (window->top_bar)
	{
		draw_rect(0, 0, window->width, 20, DISPLAY_COLORS[WHITE]);
		draw_text(2, 2, 8, 16, "12:40", DISPLAY_COLORS[MAGENTA], 0); // TODO: change topbar
	}
}

void	init_control(t_control *control, const char *name, e_control_type type)
{
	int	i = 0;
	memset(control, 0, sizeof(t_control));
	while (name[i] && i < 24)
	{
		control->name[i] = ((char *)name)[i];
		i++;
	}
	control->enabled = 1;
	control->p_type = type;
	control->visible = 1;
}
