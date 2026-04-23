#include "parsing.h"
#include "get_next_line.h"
#include "display.h"
#include "bmp.h"
#include "libft.h"

#define LAYOUT_VALUE_MAX	128
#define LAYOUT_KEY_MAX		64
#define LAYOUT_PARENT_MAX	25
#define LAYOUT_PENDING_MAX	64

typedef struct s_type_entry
{
	char		*name;
	e_control_type	type;
} t_type_entry;

typedef struct s_pending_parent
{
	t_control	*child;
	char		parent_name[LAYOUT_PARENT_MAX];
} t_pending_parent;

typedef enum e_image_mode
{
	IMAGE_MODE_STRETCH = 0,
	IMAGE_MODE_ZOOM,
	IMAGE_MODE_AUTO_SIZE,
	IMAGE_MODE_CENTER
} t_image_mode;

typedef enum e_textbox_type
{
	TEXTBOX_TYPE_NORMAL = 0,
	TEXTBOX_TYPE_PASSWORD = 1
} t_textbox_type;

static int	lp_parse_percent(char *str, int max_val)
{
	int len;
	int percent;

	if (!str || !str[0])
		return 0;
	len = ft_strlen(str);
	if (len > 0 && str[len - 1] == '%')
	{
		percent = ft_atoi(str);
		return (max_val * percent) / 100;
	}
	return ft_atoi(str);
}

static uint32_t	lp_parse_color(char *str)
{
	int r;
	int g;
	int b;
	int i;

	if (!str || !str[0])
		return 0;
	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] >= '0' && str[i] <= '9')
	{
		r = ft_atoi(str + i);
		while (str[i] && str[i] != ',')
			i++;
		if (str[i] == ',')
		{
			i++;
			while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
				i++;
			g = ft_atoi(str + i);
			while (str[i] && str[i] != ',')
				i++;
			if (str[i] == ',')
			{
				i++;
				while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
					i++;
				b = ft_atoi(str + i);
				if (r < 0) r = 0;
				if (r > 255) r = 255;
				if (g < 0) g = 0;
				if (g > 255) g = 255;
				if (b < 0) b = 0;
				if (b > 255) b = 255;
				return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
			}
		}
		return (uint32_t)r;
	}
	if (!ft_strcmp(str, "white"))
		return DISPLAY_COLORS[WHITE];
	if (!ft_strcmp(str, "black"))
		return DISPLAY_COLORS[BLACK];
	if (!ft_strcmp(str, "dark_red"))
		return DISPLAY_COLORS[DARK_RED];
	if (!ft_strcmp(str, "red"))
		return DISPLAY_COLORS[RED];
	if (!ft_strcmp(str, "orange"))
		return DISPLAY_COLORS[ORANGE];
	if (!ft_strcmp(str, "dark_blue"))
		return DISPLAY_COLORS[DARK_BLUE];
	if (!ft_strcmp(str, "green"))
		return DISPLAY_COLORS[GREEN];
	if (!ft_strcmp(str, "lime"))
		return DISPLAY_COLORS[LIME];
	if (!ft_strcmp(str, "blue"))
		return DISPLAY_COLORS[BLUE];
	if (!ft_strcmp(str, "dark_cyan"))
		return DISPLAY_COLORS[DARK_CYAN];
	if (!ft_strcmp(str, "yellow"))
		return DISPLAY_COLORS[YELLOW];
	if (!ft_strcmp(str, "cyan"))
		return DISPLAY_COLORS[CYAN];
	if (!ft_strcmp(str, "dark_magenta"))
		return DISPLAY_COLORS[DARK_MAGENTA];
	if (!ft_strcmp(str, "magenta"))
		return DISPLAY_COLORS[MAGENTA];
	if (!ft_strcmp(str, "dark_grey"))
		return DISPLAY_COLORS[DARK_GREY];
	if (!ft_strcmp(str, "grey"))
		return DISPLAY_COLORS[GREY];
	return (uint32_t)ft_atoi(str);
}

static int	lp_parse_align(char *value)
{
	if (!ft_strcmp(value, "top"))
		return ANCHOR_TOP | ANCHOR_CENTER_X;
	if (!ft_strcmp(value, "top_left"))
		return ANCHOR_TOP | ANCHOR_LEFT;
	if (!ft_strcmp(value, "top_right"))
		return ANCHOR_TOP | ANCHOR_RIGHT;
	if (!ft_strcmp(value, "bottom"))
		return ANCHOR_BOTTOM | ANCHOR_CENTER_X;
	if (!ft_strcmp(value, "bottom_left"))
		return ANCHOR_BOTTOM | ANCHOR_LEFT;
	if (!ft_strcmp(value, "bottom_right"))
		return ANCHOR_BOTTOM | ANCHOR_RIGHT;
	if (!ft_strcmp(value, "left"))
		return ANCHOR_LEFT | ANCHOR_CENTER_Y;
	if (!ft_strcmp(value, "right"))
		return ANCHOR_RIGHT | ANCHOR_CENTER_Y;
	if (!ft_strcmp(value, "center"))
		return ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
	return 0;
}

static int	lp_parse_location_xy(const char *value, t_point *out, int max_x, int max_y)
{
	int i;
	int j;
	char xbuf[32];
	char ybuf[32];

	if (!value || !out)
		return 0;
	i = 0;
	while (value[i] == ' ' || (value[i] >= 9 && value[i] <= 13))
		i++;
	j = 0;
	while (value[i] && value[i] != ',' && j < (int)sizeof(xbuf) - 1)
		xbuf[j++] = value[i++];
	xbuf[j] = '\0';
	while (value[i] && value[i] != ',')
		i++;
	if (value[i] != ',')
		return 0;
	i++;
	while (value[i] == ' ' || (value[i] >= 9 && value[i] <= 13))
		i++;
	j = 0;
	while (value[i] && value[i] != ';' && value[i] != '\n' && value[i] != '\r' && j < (int)sizeof(ybuf) - 1)
		ybuf[j++] = value[i++];
	ybuf[j] = '\0';
	if (!xbuf[0] || !ybuf[0])
		return 0;
	out->x = lp_parse_percent(xbuf, max_x);
	out->y = lp_parse_percent(ybuf, max_y);
	return 1;
}

static int	lp_parse_image_mode(char *value)
{
	if (!ft_strcmp(value, "stretch"))
		return IMAGE_MODE_STRETCH;
	if (!ft_strcmp(value, "zoom"))
		return IMAGE_MODE_ZOOM;
	if (!ft_strcmp(value, "auto_size"))
		return IMAGE_MODE_AUTO_SIZE;
	if (!ft_strcmp(value, "center"))
		return IMAGE_MODE_CENTER;
	return IMAGE_MODE_STRETCH;
}

static int	lp_parse_textbox_type(char *value)
{
	if (!ft_strcmp(value, "password"))
		return TEXTBOX_TYPE_PASSWORD;
	return TEXTBOX_TYPE_NORMAL;
}

static int	lp_parse_bool(char *str)
{
	if (!str || !str[0])
		return 0;
	if (!ft_strcmp(str, "true") || !ft_strcmp(str, "1") || !ft_strcmp(str, "yes"))
		return 1;
	return 0;
}

static int	lp_skip_ws(char *line, int i)
{
	while (line[i] == ' ' || (line[i] >= 9 && line[i] <= 13))
		i++;
	return i;
}

static int	lp_is_blank_or_comment(char *line)
{
	int i;

	i = lp_skip_ws(line, 0);
	if (!line[i] || line[i] == '\n' || line[i] == '\r' || line[i] == '#')
		return 1;
	return 0;
}

static int	lp_copy_field(char *src, char *dst, int max_len)
{
	int i;

	i = 0;
	if (max_len <= 0)
		return 0;
	while (src[i] && src[i] != '\n' && src[i] != '\r' && i < max_len - 1)
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return i;
}

static int	lp_parse_key_value(char *line, char *key, int key_max, char *value, int value_max)
{
	int i;
	int k;
	int v;
	int quoted;

	i = lp_skip_ws(line, 0);
	if (!line[i] || line[i] == '#')
		return 0;
	k = 0;
	while (line[i] && line[i] != '=' && line[i] != ' ' && line[i] != '\t' && k < key_max - 1)
		key[k++] = line[i++];
	key[k] = '\0';
	i = lp_skip_ws(line, i);
	if (line[i] != '=')
		return 0;
	i++;
	i = lp_skip_ws(line, i);
	v = 0;
	quoted = 0;
	if (line[i] == '"')
	{
		quoted = 1;
		i++;
	}
	while (line[i] && v < value_max - 1)
	{
		if (quoted)
		{
			if (line[i] == '"')
				break;
		}
		else
		{
			if (line[i] == ';' || line[i] == '\n' || line[i] == '\r' || line[i] == '#')
				break;
		}
		value[v++] = line[i++];
	}
	value[v] = '\0';
	return (k > 0 && v >= 0);
}

static int	lp_get_control_type(char *name, e_control_type *type)
{
	static const t_type_entry table[] = {
		{"Window", CONTROL_WINDOW},
		{"Text", CONTROL_TEXT},
		{"Button", CONTROL_BUTTON},
		{"CheckBox", CONTROL_CHECKBOX},
		{"Hscroll", CONTROL_HSCROLL},
		{"Image", CONTROL_IMAGE},
		{"ListView", CONTROL_LISTVIEW},
		{"ProgressBar", CONTROL_PROGRESSBAR},
		{"RadioButton", CONTROL_RADIOBUTTON},
		{"Rect", CONTROL_RECT},
		{"RoundButton", CONTROL_ROUNDBUTTON},
		{"Switch", CONTROL_SWITCH},
		{"TextBox", CONTROL_TEXTBOX},
		{"Vscroll", CONTROL_VSCROLL},
		{"WebView", CONTROL_WEBVIEW}
	};
	uint32_t i;

	if (!name || !type)
		return 0;
	for (i = 0; i < sizeof(table) / sizeof(table[0]); i++)
	{
		if (!ft_strcmp(name, table[i].name))
		{
			*type = table[i].type;
			return 1;
		}
	}
	return 0;
}

static int	lp_apply_window_attr(t_window *win, char *key, char *value)
{
	if (!ft_strcmp(key, "width"))
	{
		win->width = lp_parse_percent(value, SCREEN_WIDTH);
		return 1;
	}
	else if (!ft_strcmp(key, "height"))
	{
		win->height = lp_parse_percent(value, SCREEN_HEIGHT);
		return 1;
	}
	else if (!ft_strcmp(key, "title"))
	{
		lp_copy_field(value, win->title, sizeof(win->title));
		return 1;
	}
	else if (!ft_strcmp(key, "bg_color"))
	{
		win->bg_color = lp_parse_color(value);
		return 1;
	}
	else if (!ft_strcmp(key, "topbar"))
	{
		win->top_bar = (uint8_t)lp_parse_bool(value);
		return 1;
	}
	return 0;
}

static int	lp_apply_control_attr(t_control *control, const t_window *win, char *key, char *value)
{
	if (!ft_strcmp(key, "name"))
	{
		lp_copy_field(value, control->name, sizeof(control->name));
		return 1;
	}
	else if (!ft_strcmp(key, "content") || !ft_strcmp(key, "text"))
	{
		lp_copy_field(value, control->content, sizeof(control->content));
		return 1;
	}
	else if (!ft_strcmp(key, "width"))
	{
		control->width = lp_parse_percent(value, win ? win->width : SCREEN_WIDTH);
		return 1;
	}
	else if (!ft_strcmp(key, "height"))
	{
		control->height = lp_parse_percent(value, win ? win->height : SCREEN_HEIGHT);
		return 1;
	}
	else if (!ft_strcmp(key, "x"))
	{
		control->location.x = lp_parse_percent(value, win ? win->width : SCREEN_WIDTH);
		return 1;
	}
	else if (!ft_strcmp(key, "y"))
	{
		control->location.y = lp_parse_percent(value, win ? win->height : SCREEN_HEIGHT);
		return 1;
	}
	else if (!ft_strcmp(key, "visible"))
	{
		control->visible = (uint8_t)lp_parse_bool(value);
		return 1;
	}
	else if (!ft_strcmp(key, "enabled"))
	{
		control->enabled = (uint8_t)lp_parse_bool(value);
		return 1;
	}
	else if (!ft_strcmp(key, "color"))
	{
		control->color = lp_parse_color(value);
		return 1;
	}
	else if (!ft_strcmp(key, "bg_color"))
	{
		control->bg_color = lp_parse_color(value);
		return 1;
	}
	else if (!ft_strcmp(key, "margin_top"))
	{
		control->margin_top = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "margin_left"))
	{
		control->margin_left = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "margin_right"))
	{
		control->margin_right = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "margin_bottom"))
	{
		control->margin_bottom = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "location"))
	{
		int align;

		if (lp_parse_location_xy(value, &control->location,
				win ? win->width : SCREEN_WIDTH,
				win ? win->height : SCREEN_HEIGHT))
			return 1;
		align = lp_parse_align(value);
		if (align == 0)
			return -1;
		control->p_location_override = (e_control_anchor)align;
		return 1;
	}
	else if (!ft_strcmp(key, "text_align"))
	{
		control->text_align = lp_parse_align(value);
		return 1;
	}
	else if (!ft_strcmp(key, "font_size"))
	{
		control->font_size = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "checked"))
	{
		control->checked = lp_parse_bool(value);
		return 1;
	}
	else if (!ft_strcmp(key, "bar"))
	{
		control->bar = lp_parse_bool(value);
		return 1;
	}
	else if (!ft_strcmp(key, "scroll"))
	{
		control->scroll = lp_parse_bool(value);
		return 1;
	}
	else if (!ft_strcmp(key, "min"))
	{
		control->min = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "max"))
	{
		control->max = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "value"))
	{
		control->value = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "radius"))
	{
		control->radius = ft_atoi(value);
		return 1;
	}
	else if (!ft_strcmp(key, "mode"))
	{
		control->mode = lp_parse_image_mode(value);
		return 1;
	}
	else if (!ft_strcmp(key, "type"))
	{
		control->type = lp_parse_textbox_type(value);
		return 1;
	}
	else if (!ft_strcmp(key, "src") || !ft_strcmp(key, "image"))
	{
		bmp_load_image(&control->image, value);
		return 1;
	}
	return 0;
}

static t_control	*lp_find_control_by_name(t_control *list, char *name)
{
	t_control *found;

	while (list)
	{
		if (!ft_strcmp(list->name, name))
			return list;
		if (list->children)
		{
			found = lp_find_control_by_name(list->children, name);
			if (found)
				return found;
		}
		list = list->p_next;
	}
	return 0;
}

static void	lp_append_child(t_control *parent, t_control *child)
{
	t_control *it;

	if (!parent || !child)
		return;
	child->p_next = 0;
	if (!parent->children)
	{
		parent->children = child;
		return;
	}
	it = parent->children;
	while (it->p_next)
		it = it->p_next;
	it->p_next = child;
}

static void	lp_remove_top_level(t_window *win, t_control *target)
{
	t_control *prev;
	t_control *it;

	if (!win || !target)
		return;
	prev = 0;
	it = win->controls;
	while (it)
	{
		if (it == target)
		{
			if (prev)
				prev->p_next = it->p_next;
			else
				win->controls = it->p_next;
			it->p_next = 0;
			return;
		}
		prev = it;
		it = it->p_next;
	}
}

static int	lp_parse_block(int fd, e_control_type section_type, t_window *win, t_control *control, char *parent_name, int section_line)
{
	char	*line;
	char	key[LAYOUT_KEY_MAX];
	char	value[LAYOUT_VALUE_MAX];
	int		lines;
	int		i;
	int		apply_ret;

	lines = 0;
	if (parent_name)
		parent_name[0] = '\0';
	while ((line = get_next_line(fd)))
	{
		lines++;
		i = lp_skip_ws(line, 0);
		if (line[i] == '\0' || line[i] == '\n' || line[i] == '\r')
		{
			free(line);
			break;
		}
		if (line[i] == '#')
		{
			free(line);
			continue;
		}
		if (lp_parse_key_value(line, key, sizeof(key), value, sizeof(value)))
		{
			if (section_type == CONTROL_WINDOW)
			{
				apply_ret = lp_apply_window_attr(win, key, value);
				if (!apply_ret)
				{
					log("Unknown window config at line %i: %s\n", LOG_ERROR, section_line + lines, key);
					free(line);
					return -1;
				}
			}
			else if (parent_name && !ft_strcmp(key, "parent"))
				lp_copy_field(value, parent_name, LAYOUT_PARENT_MAX);
			else if (control)
			{
				apply_ret = lp_apply_control_attr(control, win, key, value);
				if (apply_ret == 0)
				{
					log("Unknown control config at line %i: %s\n", LOG_ERROR, section_line + lines, key);
					free(line);
					return -1;
				}
				if (apply_ret < 0)
				{
					log("Invalid control config value at line %i: %s=\"%s\"\n", LOG_ERROR, section_line + lines, key, value);
					free(line);
					return -1;
				}
			}
		}
		else
		{
			log("Invalid config syntax at line %i: %s\n", LOG_ERROR, section_line + lines, line);
			free(line);
			return -1;
		}
		free(line);
	}
	return lines;
}

int	parse_layout(const char *path, t_window *win)
{
	int			fd;
	int			line_idx;
	char		*line;
	int			i;
	int			j;
	char		section[32];
	e_control_type type;
	t_control	*control;
	t_control	*tail;
	char		parent_name[LAYOUT_PARENT_MAX];
	t_pending_parent pending[LAYOUT_PENDING_MAX];
	int		pending_count;
	int		p;
	t_control	*parent;

	if (!win)
		return 1;
	fd = open(path, O_READ);
	if (fd < 0)
	{
		log("Could not open file: %s\n", LOG_ERROR, path);
		return 1;
	}
	line_idx = 0;
	pending_count = 0;
	tail = win->controls;
	while (tail && tail->p_next)
		tail = tail->p_next;
	while ((line = get_next_line(fd)))
	{
		line_idx++;
		if (lp_is_blank_or_comment(line))
		{
			free(line);
			continue;
		}
		i = lp_skip_ws(line, 0);
		j = 0;
		while (line[i + j] && line[i + j] != ':' && line[i + j] != '\n' && line[i + j] != '\r' && j < (int)sizeof(section) - 1)
		{
			section[j] = line[i + j];
			j++;
		}
		section[j] = '\0';
		if (line[i + j] != ':')
		{
			log("Invalid layout section at line %i: %s\n", LOG_ERROR, line_idx, line);
			free(line);
			close(fd);
			return 1;
		}
		if (!lp_get_control_type(section, &type))
		{
			log("Unknown layout section at line %i: %s\n", LOG_ERROR, line_idx, section);
			free(line);
			close(fd);
			return 1;
		}
		if (type == CONTROL_WINDOW)
		{
			i = lp_parse_block(fd, CONTROL_WINDOW, win, 0, 0, line_idx);
			if (i < 0)
			{
				free(line);
				close(fd);
				return 1;
			}
			line_idx += i;
		}
		else
		{
			control = (t_control *)malloc(sizeof(t_control));
			if (!control)
			{
				log("Memory allocation failed for control\n", LOG_ERROR);
				free(line);
				close(fd);
				return 1;
			}
			init_control(control, "control", type);
			i = lp_parse_block(fd, type, win, control, parent_name, line_idx);
			if (i < 0)
			{
				free(control);
				free(line);
				close(fd);
				return 1;
			}
			line_idx += i;
			if (parent_name[0])
			{
				parent = lp_find_control_by_name(win->controls, parent_name);
				if (parent)
					lp_append_child(parent, control);
				else
				{
					if (tail)
						tail->p_next = control;
					else
						win->controls = control;
					tail = control;
					if (pending_count < LAYOUT_PENDING_MAX)
					{
						pending[pending_count].child = control;
						lp_copy_field(parent_name, pending[pending_count].parent_name, LAYOUT_PARENT_MAX);
						pending_count++;
					}
				}
			}
			else
			{
				if (tail)
					tail->p_next = control;
				else
					win->controls = control;
				tail = control;
			}
		}
		free(line);
	}
	for (p = 0; p < pending_count; p++)
	{
		parent = lp_find_control_by_name(win->controls, pending[p].parent_name);
		if (parent)
		{
			lp_remove_top_level(win, pending[p].child);
			lp_append_child(parent, pending[p].child);
		}
		else
		{
			log("Unknown parent control: %s\n", LOG_WARNING, pending[p].parent_name);
		}
	}
	close(fd);
	return 0;
}
