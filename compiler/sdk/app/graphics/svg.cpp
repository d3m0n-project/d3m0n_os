#include "app/graphics.hpp"


static int	svg_attr_int(const char *tag, int length, const char *name, int fallback)
{
	const char *found = strnstr(tag, name, length);
	if (!found)
		return fallback;

	found += strlen(name);
	while (found < tag + length && (*found == ' ' || *found == '\t' || *found == '=' || *found == '"' || *found == '\''))
		++found;

	int sign = 1;
	if (*found == '-')
	{
		sign = -1;
		++found;
	}

	int value = 0;
	int digits = 0;
	while (found < tag + length && *found >= '0' && *found <= '9')
	{
		value = value * 10 + (*found - '0');
		digits++;
		++found;
	}
	return digits ? sign * value : fallback;
}

static uint32_t	svg_color(const char *tag, int length, const char *attribute, uint32_t fallback)
{
	const char *found = strnstr(tag, attribute, length);
	if (!found)
		return fallback;

	found += strlen(attribute);
	const char *attribute_end = strchr(found, '"');
	if (!attribute_end || attribute_end > tag + length)
		attribute_end = strchr(found, '\'');

	if (attribute_end && attribute_end <= tag + length)
	{
		if (strnstr(found, "none", (int)(attribute_end - found)) || strnstr(found, "transparent", (int)(attribute_end - found)))
			return 0;
	}

	while (found < tag + length && *found != '#')
	{
		if (*found == '"' || *found == '\'')
			return fallback;
		++found;
	}

	if (found >= tag + length)
		return fallback;
	++found;

	uint32_t value = 0;
	int digits = 0;
	while (found < tag + length && digits < 6)
	{
		char c = *found++;
		int part = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
		if (part < 0)
			break;
		value = (value << 4) | part;
		digits++;
	}
	return digits == 6 ? 0xFF000000 | value : fallback;
}

static void svg_line(Display *display, int x0, int y0, int x1, int y1, uint32_t color)
{
	int dx = x1 > x0 ? x1 - x0 : x0 - x1;
	int sx = x0 < x1 ? 1 : -1;
	int dy = y1 > y0 ? y0 - y1 : y1 - y0;
	int sy = y0 < y1 ? 1 : -1;
	int error = dx + dy;
	while (true)
	{
		display->put_pixel(x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;
		int twice = 2 * error;
		if (twice >= dy)
		{
			error += dy;
			x0 += sx;
		}
		if (twice <= dx)
		{
			error += dx;
			y0 += sy;
		}
	}
}

static void svg_points(Display *display, const char *tag, int length, int ox, int oy, int sx, int sy, uint32_t color, bool close)
{
	const char *points = strnstr(tag, "points", length);
	if (!points)
		return;
	points += 6;
	int px = 0;
	int py = 0;
	int first_x = 0;
	int first_y = 0;
	bool have = false;
	while (points < tag + length)
	{
		while (points < tag + length && (*points < '0' || *points > '9') && *points != '-')
			++points;
		if (points >= tag + length)
			break;
		int nx = svg_attr_int(points, (int)(tag + length - points), "", 0);
		while (points < tag + length && *points != ',' && *points != ' ' && *points != '\t')
			++points;
		while (points < tag + length && (*points == ',' || *points == ' ' || *points == '\t'))
			++points;
		int ny = svg_attr_int(points, (int)(tag + length - points), "", 0);
		while (points < tag + length && *points != ',' && *points != ' ' && *points != '\t')
			++points;
		int dx = ox + nx * sx / 1000;
		int dy = oy + ny * sy / 1000;
		if (have)
			svg_line(display, px, py, dx, dy, color);
		else
		{
			first_x = dx;
			first_y = dy;
			have = true;
		}
		px = dx;
		py = dy;
	}
	if (close && have)
		svg_line(display, px, py, first_x, first_y, color);
}

static bool svg_next_number(const char **cursor, const char *end, int *value)
{
	const char *p = *cursor;
	while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ','))
		++p;

	if (p >= end || ((*p < '0' || *p > '9') && *p != '-' && *p != '+'))
		return false;
	
	int sign = 1;
	if (*p == '-' || *p == '+')
	{
		if (*p == '-') sign = -1;
		++p;
	}

	int result = 0;
	while (p < end && *p >= '0' && *p <= '9')
	{
		result = result * 10 + (*p - '0');
		++p;
	}
	if (p < end && *p == '.')
	{
		++p;
		int divisor = 10;
		while (p < end && *p >= '0' && *p <= '9')
		{
			result += (*p - '0') / divisor;
			divisor *= 10;
			++p;
		}
	}
	*cursor = p;
	*value = sign * result;
	return true;
}

static void svg_path(Display *display, const char *tag, int length, int ox, int oy, int sx, int sy, uint32_t color, bool filled)
{
	const char *cursor = strnstr(tag, "d", length);
	if (!cursor)
		return;
	const char *end = tag + length;
	while (cursor < end && *cursor != '=') ++cursor;
	if (cursor >= end)
		return;
	++cursor;
	while (cursor < end && *cursor != '"' && *cursor != '\'') ++cursor;
	if (cursor >= end)
		return;
	++cursor;
	int points_x[256];
	int points_y[256];
	int count = 0;
	int current_x = 0, current_y = 0, start_x = 0, start_y = 0;
	char command = 0;
	while (cursor < end && *cursor != '"' && *cursor != '\'')
	{
		if ((*cursor >= 'A' && *cursor <= 'Z') || (*cursor >= 'a' && *cursor <= 'z'))
		{
			command = *cursor++;
			if (command == 'Z' || command == 'z')
			{
				if (count > 1) svg_line(display, points_x[count - 1], points_y[count - 1], points_x[0], points_y[0], color);
				current_x = start_x;
				current_y = start_y;
			}
			continue;
		}
		int values[6];
		int needed = command == 'H' || command == 'h' || command == 'V' || command == 'v' ? 1 : command == 'C' || command == 'c' ? 6 : command == 'S' || command == 's' || command == 'Q' || command == 'q' ? 4 : 2;
		int found = 0;
		while (found < needed && svg_next_number(&cursor, end, &values[found]))
			++found;
		if (found != needed)
		{
			++cursor;
			continue;
		}
		int next_x = current_x, next_y = current_y;
		if (command == 'H' || command == 'h')
			next_x = command == 'h'?current_x + values[0] : values[0];
		else if (command == 'V' || command == 'v')
			next_y = command == 'v' ? current_y + values[0] : values[0];
		else if (command == 'C' || command == 'c')
		{
			next_x = command == 'c' ? current_x + values[4] : values[4];
			next_y = command == 'c' ? current_y + values[5] : values[5];
		}
		else if (command == 'S' || command == 's' || command == 'Q' || command == 'q')
		{
			next_x = command == 's' || command == 'q' ? current_x + values[2] : values[2];
			next_y = command == 's' || command == 'q' ? current_y + values[3] : values[3];
		}
		else
		{
			next_x = command == 'm' || command == 'l' ? current_x + values[0] : values[0];
			next_y = command == 'm' || command == 'l' ? current_y + values[1] : values[1];
		}
		int screen_x = ox + next_x * sx / 1000;
		int screen_y = oy + next_y * sy / 1000;
		if (count > 0)
			svg_line(display, points_x[count - 1], points_y[count - 1], screen_x, screen_y, color);
		if (count < 256)
		{
			points_x[count] = screen_x;
			points_y[count++] = screen_y;
		}
		current_x = next_x;
		current_y = next_y;
		if (command == 'M')
		{
			start_x = current_x;
			start_y = current_y;
			command = 'L';
		}
		if (command == 'm')
		{
			start_x = current_x;
			start_y = current_y;
			command = 'l';
		}
	}
	if (filled && count > 2)
	{
		for (int y = 0; y < display->h; ++y)
		{
			int intersections[256];
			int hits = 0;
			for (int i = 0, j = count - 1; i < count; j = i++)
				if ((points_y[i] > y) != (points_y[j] > y))
					intersections[hits++] = points_x[i] + (y - points_y[i]) * (points_x[j] - points_x[i]) / (points_y[j] - points_y[i]);
			for (int i = 0; i + 1 < hits; i += 2)
				display->draw_hline(intersections[i], y, intersections[i + 1] - intersections[i] + 1, color);
		}
	}
}

void Display::draw_svg(int x, int y, int w, int h, const char *path, uint32_t override_color)
{
	if (!path || w <= 0 || h <= 0)
		return;
	int fd = open(path, O_READ);
	if (fd < 0)
		return;
	char *svg = new char[65536];
	int total = 0;
	int amount;
	while (total < 65535 && (amount = read(fd, svg + total, 65535 - total)) > 0)
		total += amount;
	close(fd);
	if (!total)
	{
		delete[] svg;
		return;
	}
	svg[total] = '\0';
	int source_width = svg_attr_int(svg, total, "width", 320);
	int source_height = svg_attr_int(svg, total, "height", 480);
	if (source_width <= 0)
		source_width = 320;
	if (source_height <= 0)
		source_height = 480;
	const char *cursor = svg;
	while (cursor < svg + total)
	{
		const char *begin = strchr(cursor, '<');
		if (!begin)
			break;
		const char *end = strchr(begin, '>');
		if (!end)
			break;
		int length = (int)(end - begin + 1);
		uint32_t color = override_color ? override_color : svg_color(begin, length, "fill", 0xFF000000);
		if (!override_color && (strnstr(begin, "fill-opacity=\"0", length) || strnstr(begin, "opacity=\"0", length)))
			color = 0;
		int px = x + svg_attr_int(begin, length, "x", 0) * w / source_width;
		int py = y + svg_attr_int(begin, length, "y", 0) * h / source_height;
		int pw = svg_attr_int(begin, length, "width", 0) * w / source_width;
		int ph = svg_attr_int(begin, length, "height", 0) * h / source_height;
		if (strnstr(begin, "<rect", length))
			draw_rect(px, py, pw, ph, color);
		else if (strnstr(begin, "<circle", length))
			draw_ellipse(x + svg_attr_int(begin, length, "cx", 0) * w / source_width, y + svg_attr_int(begin, length, "cy", 0) * h / source_height, svg_attr_int(begin, length, "r", 0) * w / source_width, svg_attr_int(begin, length, "r", 0) * h / source_height, color, 1);
		else if (strnstr(begin, "<ellipse", length))
			draw_ellipse(x + svg_attr_int(begin, length, "cx", 0) * w / source_width, y + svg_attr_int(begin, length, "cy", 0) * h / source_height, svg_attr_int(begin, length, "rx", 0) * w / source_width, svg_attr_int(begin, length, "ry", 0) * h / source_height, color, 1);
		else if (strnstr(begin, "<line", length))
			svg_line(this, x + svg_attr_int(begin, length, "x1", 0) * w / source_width, y + svg_attr_int(begin, length, "y1", 0) * h / source_height, x + svg_attr_int(begin, length, "x2", 0) * w / source_width, y + svg_attr_int(begin, length, "y2", 0) * h / source_height, color);
		else if (strnstr(begin, "<polygon", length))
			svg_points(this, begin, length, x, y, w * 1000 / source_width, h * 1000 / source_height, color, true);
		else if (strnstr(begin, "<polyline", length))
			svg_points(this, begin, length, x, y, w * 1000 / source_width, h * 1000 / source_height, color, false);
		else if (strnstr(begin, "<path", length))
			svg_path(this, begin, length, x, y, w * 1000 / source_width, h * 1000 / source_height, color, true);
		cursor = end + 1;
	}
	delete[] svg;
}
