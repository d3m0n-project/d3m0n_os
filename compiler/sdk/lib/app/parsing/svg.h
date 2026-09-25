#ifndef SVG_H
#define SVG_H

#include "types.h"
#include "xml.h"

#ifdef __cplusplus
	#include "app/graphics.hpp"
	#include "sys.h"
	#include "stdio.hpp"
	#define DISPLAY_FUNC(name)	Display::name
	#define DISPLAY(name)		display->name
	#define PRINT(fmt, data)	printf(fmt, data);
	#define FUNC_TYPE			inline
	#include "app/HTML/html_colors.h"
#else
	#include "../compiler/sdk/lib/app/HTML/html_colors.h"
	#include "display/display.h"
	#include "filesystem/filesystem.h"
	#include "memory/memory.h"
	#include "libft.h"
	#include "time.h"
	#define strnstr(a, b, c)		ft_strnstr(a, b, c)
	#define strlen(a)				ft_strlen(a)
	#define strchr(a, b)			ft_strchr(a, b)
	#define isspace(a)				ft_isspace(a)
	#define PRINT(fmt, data)		log(fmt, 0, data);//usleep(1000000);

	#define DISPLAY_FUNC(name)		name
	#define Display					int
	#define DISPLAY(name)			name
	#define this					0
	#define malloc					kmalloc
	#define free					kfree
	#define FUNC_TYPE				inline static 
#endif

#define SVG_COORD_SCALE			10000
#define SVG_TRANSFORM_SCALE		10000000

inline static int svg_next_number(const char **cursor, const char *end, int *value);

inline static void	svg_add_point(int points_x[32][256], int points_y[32][256], int point_counts[32], int path, int x, int y)
{
	int count = point_counts[path];
	if (count > 0)
	{
		if (points_x[path][count - 1] == x && points_y[path][count - 1] == y)
			return;
	}

	if (count >= 256)
		return;

	points_x[path][count] = x;
	points_y[path][count] = y;
	point_counts[path]++;
}

inline static void svg_viewbox(const char *tag, int length, int *x, int *y, int *w, int *h)
{
	const char *p = tag;
	const char *end = tag + length;
	int viewbox_len = strlen("viewBox");

	*x = 0;
	*y = 0;
	*w = 320;  // fallback
	*h = 480;  // fallback

	while (p < end)
	{
		while (p < end && (isspace(*p) || *p == '<' || *p == '/'))
			++p;

		if (p + viewbox_len <= end)
		{
			int match = 1;

			for (int i = 0; i < viewbox_len; ++i)
			{
				if (p[i] != "viewBox"[i])
				{
					match = 0;
					break;
				}
			}

			if (match && (p + viewbox_len == end || isspace(p[viewbox_len]) || p[viewbox_len] == '='))
			{
				const char *v = p + viewbox_len;

				while (v < end && isspace(*v))
					++v;

				if (v >= end || *v != '=')
					return;

				++v;

				while (v < end && isspace(*v))
					++v;

				if (v < end && (*v == '"' || *v == '\''))
					++v;

				// parse x, y, width, height
				int values[4];
				int found = 0;
				while (found < 4 && v < end)
				{
					int num = 0;
					int sign = 1;
					int digits = 0;

					while (v < end && (*v == ' ' || *v == '\t' || *v == ','))
						++v;

					if (v >= end)
						break;

					if (*v == '-')
					{
						sign = -1;
						++v;
					}
					else if (*v == '+')
						++v;

					while (v < end && *v >= '0' && *v <= '9')
					{
						num = num * 10 + (*v - '0');
						++v;
						++digits;
					}

					if (digits)
						values[found++] = sign * num;
				}

				if (found == 4)
				{
					*x = values[0];
					*y = values[1];
					*w = values[2];
					*h = values[3];
				}
				return;
			}
		}

		// skip current attribute
		while (p < end && !isspace(*p) && *p != '=')
			++p;

		while (p < end && isspace(*p))
			++p;

		if (p < end && *p == '=')
		{
			++p;

			while (p < end && isspace(*p))
				++p;

			if (p < end && (*p == '"' || *p == '\''))
			{
				char quote = *p++;
				while (p < end && *p != quote)
					++p;
				if (p < end)
					++p;
			}
			else
			{
				while (p < end && !isspace(*p))
					++p;
			}
		}
	}
}

inline static int svg_attr_int(const char *tag, int length, const char *name, int fallback)
{
	const char *p = tag;
	const char *end = tag + length;
	int name_len = (int)strlen(name);

	while (p < end)
	{
		while (p < end && (isspace(*p) || *p == '<' || *p == '/'))
			++p;

		if (p + name_len <= end)
		{
			int match = 1;

			for (int i = 0; i < name_len; ++i)
			{
				if (p[i] != name[i])
				{
					match = 0;
					break;
				}
			}

			if (match && (p + name_len == end || isspace(p[name_len]) || p[name_len] == '='))
			{
				const char *v = p + name_len;

				while (v < end && isspace(*v))
					++v;

				if (v >= end || *v != '=')
					return fallback;

				++v;

				while (v < end && isspace(*v))
					++v;

				if (v < end && (*v == '"' || *v == '\''))
					++v;

				int sign = 1;

				if (v < end && *v == '-')
				{
					sign = -1;
					++v;
				}
				else if (v < end && *v == '+')
					++v;

				int value = 0;
				int digits = 0;
				while (v < end && *v >= '0' && *v <= '9')
				{
					value = value * 10 + (*v - '0');
					++v;
					++digits;
				}
				return digits ? sign * value : fallback;
			}
		}

		// skip current attribute
		while (p < end && !isspace(*p) && *p != '=')
			++p;

		while (p < end && isspace(*p))
			++p;

		if (p < end && *p == '=')
		{
			++p;

			while (p < end && isspace(*p))
				++p;

			if (p < end && (*p == '"' || *p == '\''))
			{
				char quote = *p++;
				while (p < end && *p != quote)
					++p;
				if (p < end)
					++p;
			}
			else
			{
				while (p < end && !isspace(*p))
					++p;
			}
		}
	}

	return fallback;
}

inline static uint32_t svg_color(const char *tag, int length, const char *attribute, uint32_t fallback)
{
	const char *p = tag;
	const char *end = tag + length;
	int attr_len = (int)strlen(attribute);
	while (p < end)
	{
		while (p < end && (isspace(*p) || *p == '<' || *p == '/'))
			++p;

		if (p + attr_len <= end)
		{
			int match = 1;
			for (int i = 0; i < attr_len; ++i)
			{
				if (p[i] != attribute[i])
				{
					match = 0;
					break;
				}
			}

			if (match && (p + attr_len == end || isspace(p[attr_len]) || p[attr_len] == '='))
			{
				const char *v = p + attr_len;
				while (v < end && isspace(*v))
					++v;

				if (v >= end || *v != '=')
					return fallback;

				++v;

				while (v < end && isspace(*v))
					++v;

				char quote = 0;
				if (v < end && (*v == '"' || *v == '\''))
					quote = *v++;

				const char *value_start = v;
				while (v < end)
				{
					if (quote)
					{
						if (*v == quote)
							break;
					}
					else if (isspace(*v) || *v == '>')
						break;

					++v;
				}

				int value_len = (int)(v - value_start);
				if (strnstr(value_start, "none", value_len) || strnstr(value_start, "transparent", value_len))
					return 0;

				uint32_t found_color_name = get_html_color_from_name(value_start, value_len);
				if (found_color_name)
					return found_color_name;

				if (value_len == 7 && value_start[0] == '#')
				{
					uint32_t value = 0;

					for (int i = 1; i < 7; ++i)
					{
						char c = value_start[i];
						int n;

						if (c >= '0' && c <= '9')
							n = c - '0';
						else if (c >= 'a' && c <= 'f')
							n = c - 'a' + 10;
						else if (c >= 'A' && c <= 'F')
							n = c - 'A' + 10;
						else
							return fallback;

						value = (value << 4) | (uint32_t)n;
					}
					return 0xFF000000 | ((value & 0x00FF0000) >> 16) | ((value & 0x000000FF) << 16);
				}
				return fallback;
			}
		}

		while (p < end && !isspace(*p) && *p != '=')
			++p;

		while (p < end && isspace(*p))
			++p;

		if (p < end && *p == '=')
		{
			++p;
			while (p < end && isspace(*p))
				++p;
			if (p < end && (*p == '"' || *p == '\''))
			{
				char quote = *p++;
				while (p < end && *p != quote)
					++p;

				if (p < end)
					++p;
			}
		}
	}
	return fallback;
}

inline static void svg_line(Display *display, int x0, int y0, int x1, int y1, uint32_t color)
{
	(void)display;
	int dx = x1 > x0 ? x1 - x0 : x0 - x1;
	int sx = x0 < x1 ? 1 : -1;
	int dy = y1 > y0 ? y1 - y0 : y0 - y1;
	int sy = y0 < y1 ? 1 : -1;
	int error = dx + dy;
	while (1)
	{
		DISPLAY(put_pixel)(x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;

		int e2 = error * 2;
		if (e2 >= dy)
		{
			error += dy;
			x0 += sx;
		}

		if (e2 <= dx)
		{
			error += dx;
			y0 += sy;
		}
	}
}

inline static void svg_points(Display *display, const char *tag, int length, int ox, int oy, int sx, int sy, uint32_t color, int close)
{
	PRINT("aaa\n", 0);
	(void)display;
	const char *points = strnstr(tag, "points", length);
	if (!points)
		return;
	points += 6;
	int px = 0;
	int py = 0;
	int first_x = 0;
	int first_y = 0;
	int have = 0;
	const char *end = tag + length;
	while (points < end)
	{
		int nx;
		int ny;
		if (!svg_next_number(&points, end, &nx))
			break;
		while (points < end && (*points == ',' || *points == ' ' || *points == '\t'))
			++points;
		if (!svg_next_number(&points, end, &ny))
			break;
		while (points < end && (*points == ',' || *points == ' ' || *points == '\t'))
			++points;
		int dx = ox + (nx * sx) / SVG_TRANSFORM_SCALE;
		int dy = oy + (ny * sy) / SVG_TRANSFORM_SCALE;
		if (have)
			svg_line(display, px, py, dx, dy, color);
		else
		{
			first_x = dx;
			first_y = dy;
			have = 1;
		}
		px = dx;
		py = dy;
	}
	if (close && have)
		svg_line(display, px, py, first_x, first_y, color);
	PRINT("px:      %i, ", px);
	PRINT("py:      %i, ", py);
	PRINT("first_x: %i, ", first_x);
	PRINT("first_y: %i\n", first_y);
}

inline static int svg_next_number(const char **cursor, const char *end, int *value)
{
	const char *p = *cursor;
	int sign = 1;
	int integer = 0;
	int fraction = 0;
	int fraction_digits = 0;

	while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ','))
		++p;

	if (p >= end || ((*p < '0' || *p > '9') && *p != '-' && *p != '+' && *p != '.'))
		return 0;

	if (*p == '-')
	{
		sign = -1;
		++p;
	}
	else if (*p == '+')
		++p;

	while (p < end && *p >= '0' && *p <= '9')
	{
		integer = integer * 10 + (*p - '0');
		++p;
	}

	if (p < end && *p == '.')
	{
		++p;
		while (p < end && *p >= '0' && *p <= '9' && fraction_digits < 4)
		{
			fraction = fraction * 10 + (*p - '0');
			++fraction_digits;
			++p;
		}
		// skip additional decimal places we don't use
		while (p < end && *p >= '0' && *p <= '9')
			++p;
	}

	int scaled = integer * SVG_COORD_SCALE;
	if (fraction_digits == 1)
		fraction *= 1000;
	else if (fraction_digits == 2)
		fraction *= 100;
	else if (fraction_digits == 3)
		fraction *= 10;
	else if (fraction_digits == 4)
		fraction *= 1;
	
	scaled += fraction;
	*value = sign * scaled;
	*cursor = p;
	return 1;
}

inline static void	svg_fill_paths(Display *display, int points_x[32][256], int points_y[32][256], int point_counts[32], int path_count, uint32_t color)
{
	(void)display;
	if (path_count <= 0 || !color)
		return;

	#ifdef __cplusplus
	int screen_width = display->w;
	int screen_height = display->h;
	#else
	int screen_width = SCREEN_WIDTH;
	int screen_height = SCREEN_HEIGHT;
	#endif

	int min_y = screen_height - 1;
	int max_y = 0;
	int have_points = 0;

	for (int path = 0; path < path_count; ++path)
	{
		int count = point_counts[path];
		if (count < 3)
			continue;

		for (int i = 0; i < count; ++i)
		{
			if (points_y[path][i] < min_y)
				min_y = points_y[path][i];

			if (points_y[path][i] > max_y)
				max_y = points_y[path][i];

			have_points = 1;
		}
	}

	if (!have_points)
		return;

	if (min_y < 0)
		min_y = 0;

	if (max_y >= screen_height)
		max_y = screen_height - 1;
	
	for (int y = min_y; y <= max_y; ++y)
	{
		int intersections[256];
		int hits = 0;

		// use doubles coords to avoid decimal point for 0.5
		int scan_y = y * 2 + 1;

		for (int path = 0; path < path_count; ++path)
		{
			int count = point_counts[path];
			if (count < 3)
				continue;

			for (int i = 0, j = count - 1; i < count; j = i++)
			{
				int x0 = points_x[path][j];
				int y0 = points_y[path][j];
				int x1 = points_x[path][i];
				int y1 = points_y[path][i];

				// ignore horizontal edges
				if (y0 == y1)
					continue;


				int y0_2 = y0 * 2;
				int y1_2 = y1 * 2;
				if (y0_2 < y1_2)
				{
					if (scan_y < y0_2 || scan_y >= y1_2)
						continue;
				}
				else
				{
					if (scan_y < y1_2 || scan_y >= y0_2)
						continue;
				}

				int64_t dx = (int64_t)x1 - x0;
				int64_t dy = (int64_t)y1 - y0;
				int64_t sy = (int64_t)scan_y - y0_2;
				int64_t x_intersect = (int64_t)x0 + (sy * dx) / (2LL * dy);
				if (hits < 256)
					intersections[hits++] = (int)x_intersect;
			}
		}

		if (hits < 2)
			continue;

		// sort intersections
		for (int i = 0; i < hits - 1; ++i)
		{
			for (int j = i + 1; j < hits; ++j)
			{
				if (intersections[i] > intersections[j])
				{
					int temp = intersections[i];
					intersections[i] = intersections[j];
					intersections[j] = temp;
				}
			}
		}

		// remove duplicates
		int unique_hits = 0;

		for (int i = 0; i < hits; ++i)
		{
			if (unique_hits == 0 || intersections[i] != intersections[unique_hits - 1])
				intersections[unique_hits++] = intersections[i];
		}


		for (int i = 0; i + 1 < unique_hits; i += 2)
		{
			int x1 = intersections[i];
			int x2 = intersections[i + 1];

			if (x1 > x2)
			{
				int temp = x1;
				x1 = x2;
				x2 = temp;
			}

			if (x2 < 0 || x1 >= screen_width)
				continue;

			if (x1 < 0)
				x1 = 0;

			if (x2 >= screen_width)
				x2 = screen_width - 1;

			if (x2 >= x1)
				DISPLAY(draw_hline)(x1, y, x2 - x1 + 1, color);
		}
	}
}

inline static void	svg_fill_polygon(Display *display, int *points_x, int *points_y, int count, uint32_t color)
{
	(void)display;
	if (count < 3)
		return;

	int min_y = points_y[0];
	int max_y = points_y[0];
	for (int i = 1; i < count; ++i)
	{
		if (points_y[i] < min_y)
			min_y = points_y[i];
		if (points_y[i] > max_y)
			max_y = points_y[i];
	}

	#ifdef __cplusplus
	int screen_height = display->h;
	#else
	int screen_height = SCREEN_HEIGHT;
	#endif

	if (min_y < 0)
		min_y = 0;
	if (max_y >= screen_height)
		max_y = screen_height - 1;


	for (int y = min_y; y <= max_y; ++y)
	{
		int intersections[256];
		int hits = 0;
		for (int i = 0, j = count - 1; i < count; j = i++)
		{
			int yi = points_y[i];
			int yj = points_y[j];
			if ((yi > y) != (yj > y))
			{
				if (yi != yj) // avoid division by zero
				{
					int xi = points_x[i];
					int xj = points_x[j];
					// calc x intersection
					int x_intersect = xi + (y - yi) * (xj - xi) / (yj - yi);
					
					if (hits < 256)
						intersections[hits++] = x_intersect;
				}
			}
		}

		// sort intersections by x
		for (int i = 0; i < hits - 1; ++i)
		{
			for (int j = i + 1; j < hits; ++j)
			{
				if (intersections[i] > intersections[j])
				{
					int temp = intersections[i];
					intersections[i] = intersections[j];
					intersections[j] = temp;
				}
			}
		}

		// fill pixels
		for (int i = 0; i + 1 < hits; i += 2)
		{
			int x1 = intersections[i];
			int x2 = intersections[i + 1];
			if (x1 < 0)
				x1 = 0;
			if (x2 < 0)
				x2 = 0;
			
			if (x2 > x1)
			{
				int width = x2 - x1 + 1;
				DISPLAY(draw_hline)(x1, y, width, color);
			}
		}
	}
}

inline static void svg_stroke_line(Display *display, int x0, int y0, int x1, int y1, uint32_t color, int width)
{
	if (!color)
		return;
	if (width < 1)
		width = 1;
	for (int offset = -(width / 2); offset <= width / 2; ++offset)
	{
		svg_line(display, x0 + offset, y0, x1 + offset, y1, color);
		svg_line(display, x0, y0 + offset, x1, y1 + offset, color);
	}
}

inline static const char *svg_path_data(const char *tag, int length)
{
	const char *p = tag;
	const char *end = tag + length;

	while (p < end)
	{
		while (p < end && (isspace(*p) || *p == '<' || *p == '/'))
			++p;
		if (p >= end)
			break;

		const char *name = p;
		while (p < end && !isspace(*p) && *p != '=' && *p != '>')
			++p;
		if (p - name == 1 && *name == 'd')
		{
			while (p < end && isspace(*p))
				++p;
			if (p < end && *p == '=')
			{
				++p;
				while (p < end && isspace(*p))
					++p;
				if (p < end && (*p == '"' || *p == '\''))
					return p + 1;
			}
		}

		while (p < end && isspace(*p))
			++p;
		if (p < end && *p == '=')
		{
			++p;
			while (p < end && isspace(*p))
				++p;
			if (p < end && (*p == '"' || *p == '\''))
			{
				char quote = *p++;
				while (p < end && *p != quote)
					++p;
				if (p < end)
					++p;
			}
			else
			{
				while (p < end && !isspace(*p) && *p != '>')
					++p;
			}
		}
	}
	return 0;
}

inline static void svg_path(Display *display, const char *tag, int length, int ox, int oy, int sx, int sy, uint32_t fill_color, uint32_t stroke_color, int stroke_width)
{
	(void)display;
	const char *cursor = svg_path_data(tag, length);
	const char *end = tag + length;
	if (!cursor)
		return;

	char quote = *(cursor - 1);
	const char *path_end = cursor;
	while (path_end < end && *path_end != quote)
		++path_end;

	end = path_end;
	typedef struct {
		int points_x[32][256];
		int points_y[32][256];
		int point_counts[32];
	}	svg_path_buffers_t;

	svg_path_buffers_t	*buffers = (svg_path_buffers_t *)malloc(sizeof(svg_path_buffers_t));
	if (!buffers)
		return;

	int path_count = 0;

	int current_x = 0;
	int current_y = 0;

	int start_x = 0;
	int start_y = 0;

	char command = 0;
	while (cursor < end)
	{
		if ((*cursor >= 'A' && *cursor <= 'Z') || (*cursor >= 'a' && *cursor <= 'z'))
		{
			command = *cursor++;
			if (command == 'Z' || command == 'z')
			{
				if (path_count > 0 && buffers->point_counts[path_count - 1] > 1)
				{
					int count = buffers->point_counts[path_count - 1];
					if (stroke_color)
						svg_stroke_line(display, buffers->points_x[path_count - 1][count - 1], buffers->points_y[path_count - 1][count - 1], buffers->points_x[path_count - 1][0], buffers->points_y[path_count - 1][0], stroke_color, stroke_width);
				}

				current_x = start_x;
				current_y = start_y;
				command = 0;
				continue;
			}
			continue;
		}

		if (!command)
		{
			++cursor;
			continue;
		}

		int needed;
		switch (command)
		{
			case 'H':
			case 'h':
			case 'V':
			case 'v':
				needed = 1;
				break;

			case 'C':
			case 'c':
				needed = 6;
				break;

			case 'Q':
			case 'q':
				needed = 4;
				break;

			case 'M':
			case 'm':
			case 'L':
			case 'l':
			default:
				needed = 2;
				break;
		}

		int values[6];
		int found = 0;
		while (found < needed)
		{
			const char *before = cursor;
			if (!svg_next_number(&cursor, end, &values[found]))
				break;

			if (cursor == before)
				break;

			++found;
		}

		if (found != needed)
		{
			++cursor;
			continue;
		}

		// create a new subpath
		if (command == 'M' || command == 'm')
		{
			int next_x;
			int next_y;

			if (command == 'm')
			{
				next_x = current_x + values[0];
				next_y = current_y + values[1];
			}
			else
			{
				next_x = values[0];
				next_y = values[1];
			}

			if (path_count >= 32)
			{
				command = command == 'm' ? 'l' : 'L';
				current_x = next_x;
				current_y = next_y;
				continue;
			}

			path_count++;
			buffers->point_counts[path_count - 1] = 0;

			start_x = next_x;
			start_y = next_y;

			int screen_x = ox + (next_x * sx) / SVG_TRANSFORM_SCALE;
			int screen_y = oy + (next_y * sy) / SVG_TRANSFORM_SCALE;

			buffers->points_x[path_count - 1][0] = screen_x;
			buffers->points_y[path_count - 1][0] = screen_y;
			buffers->point_counts[path_count - 1] = 1;

			current_x = next_x;
			current_y = next_y;
			command = command == 'm' ? 'l' : 'L';

			continue;
		}

		// no active subpath, ignore
		if (path_count <= 0)
			continue;

		int path_index = path_count - 1;

		// cubic Bezier
		if (command == 'C' || command == 'c')
		{
			int x0 = current_x;
			int y0 = current_y;

			int x1 = values[0];
			int y1 = values[1];

			int x2 = values[2];
			int y2 = values[3];

			int x3 = values[4];
			int y3 = values[5];

			if (command == 'c')
			{
				x1 += current_x;
				y1 += current_y;
				x2 += current_x;
				y2 += current_y;
				x3 += current_x;
				y3 += current_y;
			}

			for (int step = 0; step <= 16; ++step)
			{
				int t = step;
				int nt = 16 - t;

				int64_t px = ((int64_t)nt * nt * nt * x0 + 3LL * nt * nt * t * x1 + 3LL * nt * t * t * x2 + (int64_t)t * t * t * x3) / 4096;
				int64_t py = ((int64_t)nt * nt * nt * y0 + 3LL * nt * nt * t * y1 + 3LL * nt * t * t * y2 + (int64_t)t * t * t * y3) / 4096;

				int screen_x = ox + (int)((px * (int64_t)sx) / SVG_TRANSFORM_SCALE);
				int screen_y = oy + (int)((py * (int64_t)sy) / SVG_TRANSFORM_SCALE);

				int count = buffers->point_counts[path_index];
				if (count > 0 && stroke_color)
					svg_stroke_line(display, buffers->points_x[path_index][count - 1], buffers->points_y[path_index][count - 1], screen_x, screen_y, stroke_color, stroke_width);

				DISPLAY(put_pixel)(screen_x, screen_y, 0xFF00FF00);
				svg_add_point(buffers->points_x, buffers->points_y, buffers->point_counts, path_index, screen_x, screen_y);
			}

			current_x = x3;
			current_y = y3;

			continue;
		}

		// quadratic Bezier
		if (command == 'Q' || command == 'q')
		{
			int x0 = current_x;
			int y0 = current_y;

			int x1 = values[0];
			int y1 = values[1];

			int x2 = values[2];
			int y2 = values[3];

			if (command == 'q')
			{
				x1 += current_x;
				y1 += current_y;
				x2 += current_x;
				y2 += current_y;
			}

			for (int step = 0; step <= 16; ++step)
			{
				int t = step;
				int nt = 16 - t;
				int px = (nt * nt * x0 + 2 * nt * t * x1 + t * t * x2) / 256;
				int py = (nt * nt * y0 + 2 * nt * t * y1 + t * t * y2) / 256;
				int screen_x = ox + (int)((long)px * sx / SVG_TRANSFORM_SCALE);
				int screen_y = oy + (int)((long)py * sy / SVG_TRANSFORM_SCALE);
				int count = buffers->point_counts[path_index];
				if (count > 0 && stroke_color)
					svg_stroke_line(display, buffers->points_x[path_index][count - 1], buffers->points_y[path_index][count - 1], screen_x, screen_y, stroke_color, stroke_width);

				if (count < 256)
				{
					buffers->points_x[path_index][count] = screen_x;
					buffers->points_y[path_index][count] = screen_y;
					buffers->point_counts[path_index]++;
				}
			}

			current_x = x2;
			current_y = y2;
			continue;
		}

		if (command == 'H' || command == 'h')
		{
			int next_x = command == 'h' ? current_x + values[0] : values[0];
			int screen_x = ox + (next_x * sx) / SVG_TRANSFORM_SCALE;
			int screen_y = oy + (current_y * sy) / SVG_TRANSFORM_SCALE;
			int count = buffers->point_counts[path_index];
			if (count > 0 && stroke_color)
				svg_stroke_line(display, buffers->points_x[path_index][count - 1], buffers->points_y[path_index][count - 1], screen_x, screen_y, stroke_color, stroke_width);

			if (count < 256)
			{
				buffers->points_x[path_index][count] = screen_x;
				buffers->points_y[path_index][count] = screen_y;
				buffers->point_counts[path_index]++;
			}

			current_x = next_x;
			continue;
		}

		if (command == 'V' || command == 'v')
		{
			int next_y = command == 'v' ? current_y + values[0] : values[0];
			int screen_x = ox + (current_x * sx) / SVG_TRANSFORM_SCALE;
			int screen_y = oy + (next_y * sy) / SVG_TRANSFORM_SCALE;
			int count = buffers->point_counts[path_index];
			if (count > 0 && stroke_color)
				svg_stroke_line(display, buffers->points_x[path_index][count - 1], buffers->points_y[path_index][count - 1], screen_x, screen_y, stroke_color, stroke_width);

			if (count < 256)
			{
				buffers->points_x[path_index][count] = screen_x;
				buffers->points_y[path_index][count] = screen_y;
				buffers->point_counts[path_index]++;
			}

			current_y = next_y;
			continue;
		}

		int next_x;
		int next_y;
		if (command == 'l')
		{
			next_x = current_x + values[0];
			next_y = current_y + values[1];
		}
		else
		{
			next_x = values[0];
			next_y = values[1];
		}

		int screen_x = ox + (next_x * sx) / SVG_TRANSFORM_SCALE;
		int screen_y = oy + (next_y * sy) / SVG_TRANSFORM_SCALE;
		int count = buffers->point_counts[path_index];
		if (count > 0 && stroke_color)
			svg_stroke_line(display, buffers->points_x[path_index][count - 1], buffers->points_y[path_index][count - 1], screen_x, screen_y, stroke_color, stroke_width);
		if (count < 256)
		{
			buffers->points_x[path_index][count] = screen_x;
			buffers->points_y[path_index][count] = screen_y;
			buffers->point_counts[path_index]++;
		}

		current_x = next_x;
		current_y = next_y;
	}

	// fill all contours
	if (fill_color && path_count > 0)
		svg_fill_paths(display, buffers->points_x, buffers->points_y, buffers->point_counts, path_count, fill_color);
}

FUNC_TYPE void	DISPLAY_FUNC(draw_svg_buff)(int x, int y, int w, int h, const char *svg, size_t size, uint32_t override_color)
{
	int viewbox_x, viewbox_y, viewbox_w, viewbox_h;
	svg_viewbox(svg, size, &viewbox_x, &viewbox_y, &viewbox_w, &viewbox_h);
	
	int source_width = viewbox_w;
	int source_height = viewbox_h;
	
	if (source_width <= 0)
		source_width = 320;
	if (source_height <= 0)
		source_height = 480;

	const char *cursor = svg;
	while (cursor < svg + size)
	{
		const char *begin = strchr(cursor, '<');
		if (!begin)
			break;
		const char *end = strchr(begin, '>');
		if (!end)
			break;
		int length = (int)(end - begin + 1);
		uint32_t fill_color = svg_color(begin, length, "fill", 0xff000000);
		uint32_t stroke_color = svg_color(begin, length, "stroke", 0);
		int has_stroke = strnstr(begin, "stroke=\"", length) || strnstr(begin, "stroke='", length);
		int stroke_width = svg_attr_int(begin, length, "stroke-width", 1);
		if (fill_color == 0x01000000)
			fill_color = 0xFF000000;
		if (stroke_width < 1)
			stroke_width = 1;
		if (!fill_color && !stroke_color)
		{
			cursor = end + 1;
			continue;
		}
		if (strnstr(begin, "fill-opacity=\"0", length) || strnstr(begin, "fill-opacity='0", length) || strnstr(begin, "opacity='0", length) || strnstr(begin, "opacity=\"0", length))
		{
			cursor = end + 1;
			continue;
		} // TODO: handle opacity
		if (override_color)
		{
			if (fill_color)
				fill_color = override_color;
			if (has_stroke && stroke_color)
				stroke_color = override_color;
		}
		int px = x + svg_attr_int(begin, length, "x", 0) * w / source_width;
		int py = y + svg_attr_int(begin, length, "y", 0) * h / source_height;
		int pw = svg_attr_int(begin, length, "width", 0) * w / source_width;
		int ph = svg_attr_int(begin, length, "height", 0) * h / source_height;
		PRINT("begin = '%s'\n", begin);
		if (strnstr(begin, "<rect", length))
		{
			int rx = svg_attr_int(begin, length, "rx", 0);
			draw_rounded_rect(px, py, pw, ph, rx * w / source_width, fill_color);
		}
		else if (strnstr(begin, "<circle", length))
		{
			int cx = svg_attr_int(begin, length, "cx", 0);
			int cy = svg_attr_int(begin, length, "cy", 0);
			int r = svg_attr_int(begin, length, "r", 0);
			draw_ellipse(
				x + cx * w / source_width,
				y + cy * h / source_height,
				r * w / source_width,
				r * h / source_height,
				fill_color,
				1
			);
		}
		else if (strnstr(begin, "<ellipse", length))
		{
			int cx = svg_attr_int(begin, length, "cx", 0);
			int cy = svg_attr_int(begin, length, "cy", 0);
			int rx = svg_attr_int(begin, length, "rx", 0);
			int ry = svg_attr_int(begin, length, "ry", 0);
			draw_ellipse(
				x + cx * w / source_width,
				y + cy * h / source_height,
				rx * w / source_width,
				ry * h / source_height,
				fill_color,
				1
			);
		}
		else if (strnstr(begin, "<line", length))
			svg_stroke_line(this, x + svg_attr_int(begin, length, "x1", 0) * w / source_width, y + svg_attr_int(begin, length, "y1", 0) * h / source_height, x + svg_attr_int(begin, length, "x2", 0) * w / source_width, y + svg_attr_int(begin, length, "y2", 0) * h / source_height, stroke_color ? stroke_color : fill_color, stroke_width);
		else if (strnstr(begin, "<polygon", length))
			svg_points(this, begin, length, x, y, w * 1000 / source_width, h * 1000 / source_height, fill_color, 1);
		else if (strnstr(begin, "<polyline", length))
			svg_points(this, begin, length, x, y, w * 1000 / source_width, h * 1000 / source_height, stroke_color ? stroke_color : fill_color, 0);
		else if (strnstr(begin, "<path", length))
			svg_path(this, begin, length, x, y, w * 1000 / source_width, h * 1000 / source_height, fill_color, stroke_color, stroke_width);
		cursor = end + 1;
	}
}

FUNC_TYPE int	DISPLAY_FUNC(draw_svg)(int x, int y, int w, int h, const char *path, uint32_t override_color)
{
	if (!path || w <= 0 || h <= 0)
		return 1;
	int fd = open(path, O_READ);
	if (fd < 0)
		return 1;
	char *svg = (char *)malloc(65536);
	if (!svg)
		return 1;

	int total = 0;
	int amount;
	while (total < 65535 && (amount = read(fd, svg + total, 65535 - total)) > 0)
		total += amount;
	close(fd);
	if (!total)
	{
		free(svg);
		return 1;
	}
	svg[total] = '\0';
	draw_svg_buff(x, y, w, h, svg, total, override_color);
	free(svg);
	return 0;
}

#endif