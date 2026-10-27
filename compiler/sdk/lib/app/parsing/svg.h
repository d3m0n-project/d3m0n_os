#ifndef SVG_H
#define SVG_H

#include "types.h"
#include "xml.h"

#ifdef __cplusplus
#include "app/graphics.hpp"
#include "stdio.hpp"
#include "sys.h"
#define DISPLAY_FUNC(name)					Display::name
#define SVG_FUNC							inline
#define SVG_PIXEL(x, y, col)				svg_put_pixel(c, (x), (y), (col))
#define SVG_RECT(x, y, w, h, col)			c->display->draw_rect((x), (y), (w), (h), (col))
#define SVG_ELLIPSE(x, y, rx, ry, col, f)	c->display->draw_ellipse((x), (y), (rx), (ry), (col), (f))
#include "app/HTML/html_colors.h"
#else
#include "../compiler/sdk/lib/app/HTML/html_colors.h"
#include "display/display.h"
#include "filesystem/filesystem.h"
#include "memory/memory.h"
#include "time.h"
#define malloc								kmalloc
#define free								kfree
#define DISPLAY_FUNC(name)					name
#define SVG_FUNC							inline static
#define SVG_PIXEL(x, y, c)					put_pixel((x), (y), (c))
#define SVG_RECT(x, y, w, h, c)				draw_rect((x), (y), (w), (h), (c))
#define SVG_ELLIPSE(x, y, rx, ry, c, f)		draw_ellipse((x), (y), (rx), (ry), (c), (f))
#endif

#include "png.h"

#define SVG_TOKEN_COUNT 1024
#define SVG_BUFFER_LIMIT 262144

typedef struct s_svg_ctx
{
#ifdef __cplusplus
	Display *display;
#endif
	const char *buffer;
	const sxmltok_t *tokens;
	unsigned count;
	int x, y, w, h;
	double vx, vy, vw, vh;
	double a, b, cc, d, e, f;
	uint32_t override_color;
} t_svg_ctx;

static inline int svg_x(const t_svg_ctx *c, double v);
static inline int svg_y(const t_svg_ctx *c, double v);
static inline int svg_lenx(const t_svg_ctx *c, double v);
static inline int svg_leny(const t_svg_ctx *c, double v);

static inline void svg_put_pixel(t_svg_ctx *c, int x, int y, uint32_t color)
{
#ifdef __cplusplus
	unsigned alpha = color >> 24;
	if (!alpha)
		return;
	if (alpha < 255)
	{
		uint32_t background = c->display->get_pixel(x, y);
		uint32_t blended = 0xff000000;
		for (unsigned shift = 0; shift < 24; shift += 8)
		{
			unsigned foreground_channel = (color >> shift) & 0xff;
			unsigned background_channel = (background >> shift) & 0xff;
			unsigned channel = (foreground_channel * alpha + background_channel * (255 - alpha)) / 255;
			blended |= channel << shift;
		}
		color = blended;
	}
	c->display->put_pixel(x, y, color);
#else
	(void)c;
	(void)x;
	(void)y;
	(void)color;
#endif
}

static inline int svg_eq(const char *a, unsigned n, const char *b)
{
	unsigned i = 0;
	while (i < n && b[i] && a[i] == b[i])
		++i;
	return i == n && b[i] == 0;
}

static inline double svg_number(const char *s, unsigned n, double fallback)
{
	double v = 0, scale = 0.1;
	unsigned i = 0;
	int sign = 1, fraction = 0, seen = 0;
	while (i < n && (s[i] == ' ' || s[i] == '\t'))
		i++;
	if (i < n && (s[i] == '-' || s[i] == '+'))
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	for (; i < n; i++)
	{
		if (s[i] == '.')
		{
			fraction = 1;
			continue;
		}
		if (s[i] < '0' || s[i] > '9')
			break;
		seen = 1;
		if (fraction)
		{
			v += (s[i] - '0') * scale;
			scale *= 0.1;
		}
		else
			v = v * 10 + (s[i] - '0');
	}
	return seen ? v * sign : fallback;
}

static inline double svg_cnumber(const char *s)
{
	return svg_number(s, (unsigned)strlen(s), 0);
}

static inline int svg_attr(const t_svg_ctx *c, unsigned tag, const char *name, double *value)
{
	unsigned i = tag + 1, end = tag + c->tokens[tag].size + 1;
	while (i < end && i < c->count)
	{
		if (c->tokens[i].type != SXML_CDATA)
		{
			++i;
			continue;
		}
		const sxmltok_t *key = &c->tokens[i++];
		if (svg_eq(c->buffer + key->startpos, key->endpos - key->startpos, name) && i < end && c->tokens[i].type == SXML_CHARACTER)
		{
			*value = svg_number(c->buffer + c->tokens[i].startpos, c->tokens[i].endpos - c->tokens[i].startpos, 0);
			return 1;
		}
		while (i < end && c->tokens[i].type != SXML_CDATA)
			++i;
	}
	return 0;
}

static inline void svg_attr_text(const t_svg_ctx *c, unsigned tag, const char *name, char *out, unsigned cap)
{
	unsigned i = tag + 1, end = tag + c->tokens[tag].size + 1;
	if (!cap)
		return;
	out[0] = 0;
	while (i < end && i < c->count)
	{
		if (c->tokens[i].type != SXML_CDATA)
		{
			++i;
			continue;
		}
		const sxmltok_t *key = &c->tokens[i++];
		if (svg_eq(c->buffer + key->startpos, key->endpos - key->startpos, name) && i < end)
		{
			const sxmltok_t *v = &c->tokens[i];
			unsigned n = v->endpos - v->startpos;
			if (n >= cap)
				n = cap - 1;
			for (unsigned j = 0; j < n; j++)
				out[j] = c->buffer[v->startpos + j];
			out[n] = 0;
			return;
		}
		while (i < end && c->tokens[i].type != SXML_CDATA)
			++i;
	}
}

static inline uint32_t svg_color_attr(const t_svg_ctx *c, unsigned tag, const char *attribute)
{
	char s[64];
	svg_attr_text(c, tag, attribute, s, sizeof(s));
	if (!s[0])
		return attribute[0] == 'f' ? (c->override_color ? c->override_color : 0xff000000) : 0;
	if (svg_eq(s, (unsigned)strlen(s), "none"))
		return 0;
	if (c->override_color)
		return c->override_color;
	if (svg_eq(s, (unsigned)strlen(s), "black"))
		return 0xff000000;
	if (s[0] == '#')
	{
		unsigned long v = 0;
		for (unsigned i = 1; s[i]; i++)
		{
			char ch = s[i];
			unsigned d = 0;
			if (ch >= '0' && ch <= '9')
				d = ch - '0';
			else if (ch >= 'a' && ch <= 'f')
				d = ch - 'a' + 10;
			else if (ch >= 'A' && ch <= 'F')
				d = ch - 'A' + 10;
			v = (v << 4) | d;
		}
		if (strlen(s) == 4)
			return 0xff000000 | ((v & 15) * 17 << 16) | (((v >> 4) & 15) * 17 << 8) | (((v >> 8) & 15) * 17);
		return 0xff000000 | ((v & 0xff) << 16) | (v & 0xff00) | ((v >> 16) & 0xff);
	}
	for (unsigned i = 0; i < HTML_COLORS_COUNT; i++)
	{
		if (strncmp_no_alpha((char *)s, (char *)html_colors_list[i].name, strlen(s)) == 0 && strlen(s) == strlen(html_colors_list[i].name))
		{
			const int *rgb = html_colors_list[i].value;
			return 0xff000000 | ((uint32_t)rgb[2] << 16) | ((uint32_t)rgb[1] << 8) | (uint32_t)rgb[0];
		}
	}
	return 0xff000000;
}

static inline uint32_t svg_fill_color(const t_svg_ctx *c, unsigned tag)
{
	uint32_t color = svg_color_attr(c, tag, "fill");
	double opacity = 1.0, value;
	if (!color)
		return 0;
	if (svg_attr(c, tag, "opacity", &value))
		opacity = value;
	if (svg_attr(c, tag, "fill-opacity", &value))
		opacity *= value;
	if (opacity < 0)
		opacity = 0;
	if (opacity > 1)
		opacity = 1;
	return (color & 0x00ffffff) | ((uint32_t)(opacity * 255) << 24);
}

static inline uint32_t svg_stroke_color(const t_svg_ctx *c, unsigned tag)
{
	uint32_t color = svg_color_attr(c, tag, "stroke");
	double opacity = 1.0, value;
	if (!color)
		return 0;
	if (svg_attr(c, tag, "opacity", &value))
		opacity = value;
	if (svg_attr(c, tag, "stroke-opacity", &value))
		opacity *= value;
	if (opacity < 0)
		opacity = 0;
	if (opacity > 1)
		opacity = 1;
	return (color & 0x00ffffff) | ((uint32_t)(opacity * 255) << 24);
}

static inline int svg_attr_span(const t_svg_ctx *c, unsigned tag, const char *name, unsigned *start, unsigned *end)
{
	unsigned i = tag + 1, limit = tag + c->tokens[tag].size + 1;
	while (i < limit && i < c->count)
	{
		if (c->tokens[i].type != SXML_CDATA)
		{
			++i;
			continue;
		}
		const sxmltok_t *key = &c->tokens[i++];
		if (svg_eq(c->buffer + key->startpos, key->endpos - key->startpos, name) && i < limit)
		{
			*start = c->tokens[i].startpos;
			*end = c->tokens[i].endpos;
			return 1;
		}
		while (i < limit && c->tokens[i].type != SXML_CDATA)
			++i;
	}
	return 0;
}

static inline int svg_base64_value(char value)
{
	if (value >= 'A' && value <= 'Z') return value - 'A';
	if (value >= 'a' && value <= 'z') return value - 'a' + 26;
	if (value >= '0' && value <= '9') return value - '0' + 52;
	if (value == '+') return 62;
	if (value == '/') return 63;
	return -1;
}

static inline unsigned char *svg_decode_base64(const char *source, unsigned length, size_t *decoded_length)
{
	unsigned char *decoded = (unsigned char *)malloc((size_t)length * 3 / 4 + 4);
	if (!decoded)
		return NULL;
	unsigned accumulator = 0, bit_count = 0;
	*decoded_length = 0;
	for (unsigned i = 0; i < length; i++)
	{
		if (source[i] == '=')
			break;
		int value = svg_base64_value(source[i]);
		if (value < 0)
			continue;
		accumulator = (accumulator << 6) | (unsigned)value;
		bit_count += 6;
		if (bit_count >= 8)
		{
			bit_count -= 8;
			decoded[(*decoded_length)++] = (unsigned char)(accumulator >> bit_count);
			accumulator &= (1U << bit_count) - 1;
		}
	}
	return decoded;
}

static inline void svg_render_image(t_svg_ctx *c, unsigned tag)
{
	unsigned href_start, href_end;
	if (!svg_attr_span(c, tag, "xlink:href", &href_start, &href_end) && !svg_attr_span(c, tag, "href", &href_start, &href_end))
		return;
	const char *source = c->buffer + href_start;
	unsigned source_length = href_end - href_start;
	if (source_length >= 5 && source[0] == 'd')
	{
		while (source_length && *source != ',')
		{
			++source;
			--source_length;
		}
		if (source_length)
		{
			++source;
			--source_length;
		}
	}
	size_t png_size;
	unsigned char *png_data = svg_decode_base64(source, source_length, &png_size);
	if (!png_data)
		return;
	uint32_t image_width, image_height, *pixels;
	if (!png_decode(png_data, png_size, &image_width, &image_height, &pixels))
	{
		free(png_data);
		return;
	}
	free(png_data);
	double x = 0, y = 0, width = image_width, height = image_height;
	svg_attr(c, tag, "x", &x);
	svg_attr(c, tag, "y", &y);
	svg_attr(c, tag, "width", &width);
	svg_attr(c, tag, "height", &height);
	int destination_x = svg_x(c, x), destination_y = svg_y(c, y);
	int destination_width = svg_lenx(c, width), destination_height = svg_leny(c, height);
	if (destination_width > 0 && destination_height > 0 && destination_width <= 4096 && destination_height <= 4096)
	{
		for (int py = 0; py < destination_height; py++)
		{
			unsigned source_y = (unsigned)((unsigned long long)py * image_height / destination_height);
			for (int px = 0; px < destination_width; px++)
			{
				unsigned source_x = (unsigned)((unsigned long long)px * image_width / destination_width);
				SVG_PIXEL(destination_x + px, destination_y + py, pixels[source_y * image_width + source_x]);
			}
		}
	}
	free(pixels);
}

static inline int svg_x(const t_svg_ctx *c, double v)
{
	return c->x + (int)((v * c->a + c->e - c->vx) * c->w / c->vw);
}
static inline int svg_y(const t_svg_ctx *c, double v)
{
	return c->y + (int)((v * c->d + c->f - c->vy) * c->h / c->vh);
}
static inline int svg_lenx(const t_svg_ctx *c, double v)
{
	return (int)(v * c->a * c->w / c->vw);
}
static inline int svg_leny(const t_svg_ctx *c, double v)
{
	return (int)(v * c->d * c->h / c->vh);
}
static inline void svg_transform_point(const t_svg_ctx *c, double x, double y, int *px, int *py)
{
	*px = c->x + (int)((c->a * x + c->cc * y + c->e - c->vx) * c->w / c->vw);
	*py = c->y + (int)((c->b * x + c->d * y + c->f - c->vy) * c->h / c->vh);
}
static inline void svg_fill_polygon(t_svg_ctx *c, const int *px, const int *py, unsigned count, uint32_t color)
{
	int *intersections = (int *)malloc(count * sizeof(*intersections));
	if (!intersections)
		return;
	int miny = py[0], maxy = py[0];
	for (unsigned i = 1; i < count; i++)
	{
		if (py[i] < miny)
			miny = py[i];
		if (py[i] > maxy)
			maxy = py[i];
	}
	if (miny < c->y)
		miny = c->y;
	if (maxy > c->y + c->h)
		maxy = c->y + c->h;
	for (int y = miny; y < maxy; y++)
	{
		unsigned n = 0;
		for (unsigned i = 0, j = count - 1; i < count; j = i++)
		{
			if ((py[i] > y) != (py[j] > y))
			{
				intersections[n++] = px[i] + (int)((long long)(y - py[i]) * (px[j] - px[i]) / (py[j] - py[i]));
			}
		}
		for (unsigned i = 1; i < n; i++)
		{
			int v = intersections[i];
			unsigned j = i;
			while (j && intersections[j - 1] > v)
			{
				intersections[j] = intersections[j - 1];
				j--;
			}
			intersections[j] = v;
		}
		for (unsigned i = 0; i + 1 < n; i += 2)
		{
			int left = intersections[i], right = intersections[i + 1];
			if (left < c->x)
				left = c->x;
			if (right > c->x + c->w)
				right = c->x + c->w;
			for (int x = left; x <= right; x++)
				SVG_PIXEL(x, y, color);
		}
	}
	free(intersections);
}
static inline void svg_line_width(t_svg_ctx *c, int x0, int y0, int x1, int y1, uint32_t color, unsigned width)
{
	(void)c;
	if (!width)
		return;
	int radius = (int)(width / 2);
	int dx = (x1 > x0)?(x1 - x0):(x0 - x1);
	int sx = (x0 < x1)?1:-1;
	int dy = -((y1 > y0)?(y1 - y0):(y0 - y1));
	int sy = (y0 < y1)?1:-1;
	int e = dx + dy;
	for (;;)
	{
		for (int oy = -radius; oy <= radius; oy++)
			for (int ox = -radius; ox <= radius; ox++)
				SVG_PIXEL(x0 + ox, y0 + oy, color);
		if (x0 == x1 && y0 == y1)
			break;
		int e2 = 2 * e;
		if (e2 >= dy)
		{
			e += dy;
			x0 += sx;
		}
		if (e2 <= dx)
		{
			e += dx;
			y0 += sy;
		}
	}
}

static inline void svg_line(t_svg_ctx *c, int x0, int y0, int x1, int y1, uint32_t color)
{
	svg_line_width(c, x0, y0, x1, y1, color, 1);
}

static inline unsigned svg_stroke_width(const t_svg_ctx *c, unsigned tag)
{
	double width = 1, scale_x = (c->a < 0 ? -c->a : c->a) + (c->b < 0 ? -c->b : c->b), scale_y = (c->cc < 0 ? -c->cc : c->cc) + (c->d < 0 ? -c->d : c->d);
	svg_attr(c, tag, "stroke-width", &width);
	width *= (scale_x * c->w / c->vw + scale_y * c->h / c->vh) * 0.5;
	if (width < 1)
		return 1;
	if (width > 32)
		return 32;
	return (unsigned)(width + 0.5);
}

static inline void svg_render_tag(t_svg_ctx *c, unsigned i)
{
	const sxmltok_t *t = &c->tokens[i];
	const char *name = c->buffer + t->startpos;
	unsigned n = t->endpos - t->startpos;
	double a = 0, b = 0, d = 0, e = 0;
	uint32_t fill = svg_fill_color(c, i);
	uint32_t stroke = svg_stroke_color(c, i);
	if (!fill && !stroke)
		return;
	if (svg_eq(name, n, "rect"))
	{
		svg_attr(c, i, "x", &a);
		svg_attr(c, i, "y", &b);
		svg_attr(c, i, "width", &d);
		svg_attr(c, i, "height", &e);
		if (fill)
			SVG_RECT(svg_x(c, a), svg_y(c, b), svg_lenx(c, d), svg_leny(c, e), fill);
		if (stroke)
		{
			int x1 = svg_x(c, a), y1 = svg_y(c, b), x2 = x1 + svg_lenx(c, d), y2 = y1 + svg_leny(c, e);
			svg_line(c, x1, y1, x2, y1, stroke);
			svg_line(c, x2, y1, x2, y2, stroke);
			svg_line(c, x2, y2, x1, y2, stroke);
			svg_line(c, x1, y2, x1, y1, stroke);
		}
	}
	else if (svg_eq(name, n, "circle"))
	{
		svg_attr(c, i, "cx", &a);
		svg_attr(c, i, "cy", &b);
		svg_attr(c, i, "r", &d);
		if (fill)
			SVG_ELLIPSE(svg_x(c, a), svg_y(c, b), svg_lenx(c, d), svg_leny(c, d), fill, 1);
		if (stroke)
			SVG_ELLIPSE(svg_x(c, a), svg_y(c, b), svg_lenx(c, d), svg_leny(c, d), stroke, 0);
	}
	else if (svg_eq(name, n, "ellipse"))
	{
		svg_attr(c, i, "cx", &a);
		svg_attr(c, i, "cy", &b);
		svg_attr(c, i, "rx", &d);
		svg_attr(c, i, "ry", &e);
		if (fill)
			SVG_ELLIPSE(svg_x(c, a), svg_y(c, b), svg_lenx(c, d), svg_leny(c, e), fill, 1);
		if (stroke)
			SVG_ELLIPSE(svg_x(c, a), svg_y(c, b), svg_lenx(c, d), svg_leny(c, e), stroke, 0);
	}
	else if (svg_eq(name, n, "line"))
	{
		svg_attr(c, i, "x1", &a);
		svg_attr(c, i, "y1", &b);
		svg_attr(c, i, "x2", &d);
		svg_attr(c, i, "y2", &e);
		if (stroke)
			svg_line(c, svg_x(c, a), svg_y(c, b), svg_x(c, d), svg_y(c, e), stroke);
	}
	else if (svg_eq(name, n, "polygon") || svg_eq(name, n, "polyline"))
	{
		unsigned points_start, points_end;
		if (!svg_attr_span(c, i, "points", &points_start, &points_end))
			return;
		unsigned points_length = points_end - points_start;
		unsigned capacity = points_length / 3 + 1;
		int *xs = (int *)malloc((size_t)capacity * sizeof(*xs));
		int *ys = (int *)malloc((size_t)capacity * sizeof(*ys));
		if (!xs || !ys)
		{
			free(xs);
			free(ys);
			return;
		}
		unsigned count = 0;
		const char *p = c->buffer + points_start;
		const char *points_end_ptr = c->buffer + points_end;
		int firstx = 0, firsty = 0;
		while (p < points_end_ptr && count < capacity)
		{
			while (p < points_end_ptr && (*p == ',' || *p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
				p++;
			if (p == points_end_ptr)
				break;
			double xx = svg_cnumber(p);
			while (p < points_end_ptr && *p != ',' && *p != ' ' && *p != '\n' && *p != '\r' && *p != '\t')
				p++;
			while (p < points_end_ptr && (*p == ',' || *p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
				p++;
			if (p == points_end_ptr)
				break;
			double yy = svg_cnumber(p);
			while (p < points_end_ptr && *p != ',' && *p != ' ' && *p != '\n' && *p != '\r' && *p != '\t')
				p++;
			int qx, qy;
			svg_transform_point(c, xx, yy, &qx, &qy);
			if (!count)
			{
				firstx = qx;
				firsty = qy;
			}
			xs[count] = qx;
			ys[count] = qy;
			count++;
		}
		if (count > 2 && svg_eq(name, n, "polygon"))
		{
			if (fill)
				svg_fill_polygon(c, xs, ys, count, fill);
			if (stroke)
			{
				unsigned stroke_width = svg_stroke_width(c, i);
				for (unsigned j = 1; j < count; j++)
					svg_line_width(c, xs[j - 1], ys[j - 1], xs[j], ys[j], stroke, stroke_width);
				svg_line_width(c, xs[count - 1], ys[count - 1], firstx, firsty, stroke, stroke_width);
			}
		}
		else if (count > 1 && stroke)
		{
			unsigned stroke_width = svg_stroke_width(c, i);
			for (unsigned j = 1; j < count; j++)
				svg_line_width(c, xs[j - 1], ys[j - 1], xs[j], ys[j], stroke, stroke_width);
		}
		free(xs);
		free(ys);
	}
	else if (svg_eq(name, n, "image"))
		svg_render_image(c, i);
}

static inline int svg_is_supported_tag(const char *name, unsigned length)
{
	return svg_eq(name, length, "svg") || svg_eq(name, length, "g") || svg_eq(name, length, "rect") || svg_eq(name, length, "circle") || svg_eq(name, length, "ellipse") || svg_eq(name, length, "line") || svg_eq(name, length, "polygon") || svg_eq(name, length, "polyline") || svg_eq(name, length, "image");
}

static inline unsigned svg_skip_element(const t_svg_ctx *c, unsigned start, unsigned end)
{
	unsigned depth = 1;
	for (unsigned i = start + 1; i < end; i++)
	{
		if (c->tokens[i].type == SXML_STARTTAG)
			depth++;
		else if (c->tokens[i].type == SXML_ENDTAG && --depth == 0)
			return i;
	}
	return end;
}

static inline void svg_parse_transform(t_svg_ctx *c, const char *transform)
{
	const char *p = transform;
	while (*p)
	{
		int is_matrix = 0, is_translate = 0, is_scale = 0;
		if (strncmp(p, "matrix", 6) == 0) { p += 6; is_matrix = 1; }
		else if (strncmp(p, "translate", 9) == 0) { p += 9; is_translate = 1; }
		else if (strncmp(p, "scale", 5) == 0) { p += 5; is_scale = 1; }
		else { ++p; continue; }
		while (*p && *p != '(')
			++p;
		if (!*p)
			break;
		++p;
		double values[6] = {0, 0, 0, 0, 0, 0};
		unsigned value_count = 0;
		while (*p && *p != ')' && value_count < 6)
		{
			while (*p == ' ' || *p == '\t' || *p == '\n' || *p == ',') ++p;
			if (!*p || *p == ')') break;
			values[value_count++] = svg_cnumber(p);
			while (*p && *p != ')' && *p != ',' && *p != ' ' && *p != '\t' && *p != '\n') ++p;
		}
		if (*p == ')')
			++p;
		double la = 1, lb = 0, lc = 0, ld = 1, le = 0, lf = 0;
		if (is_matrix && value_count == 6) { la = values[0]; lb = values[1]; lc = values[2]; ld = values[3]; le = values[4]; lf = values[5]; }
		else if (is_translate && value_count) { le = values[0]; lf = value_count > 1 ? values[1] : 0; }
		else if (is_scale && value_count) { la = values[0]; ld = value_count > 1 ? values[1] : values[0]; }
		double a = c->a * la + c->cc * lb, b = c->b * la + c->d * lb;
		double cc = c->a * lc + c->cc * ld, d = c->b * lc + c->d * ld;
		double e = c->a * le + c->cc * lf + c->e, f = c->b * le + c->d * lf + c->f;
		c->a = a; c->b = b; c->cc = cc; c->d = d; c->e = e; c->f = f;
	}
}

static inline void svg_draw_tree(t_svg_ctx *c, unsigned begin, unsigned end)
{
	double saved_a[64], saved_b[64], saved_c[64], saved_d[64], saved_e[64], saved_f[64];
	unsigned group_depth = 0;
	for (unsigned i = begin; i < end; i++)
	{
		const sxmltok_t *token = &c->tokens[i];
		if (token->type == SXML_STARTTAG)
		{
			const char *name = c->buffer + token->startpos;
			unsigned name_length = token->endpos - token->startpos;
			if (!svg_is_supported_tag(name, name_length))
			{
				i = svg_skip_element(c, i, end);
				continue;
			}
			if (svg_eq(name, name_length, "g") && group_depth < 64)
			{
				saved_a[group_depth] = c->a;
				saved_b[group_depth] = c->b;
				saved_c[group_depth] = c->cc;
				saved_d[group_depth] = c->d;
				saved_e[group_depth] = c->e;
				saved_f[group_depth] = c->f;
				group_depth++;
				char transform[128];
				svg_attr_text(c, i, "transform", transform, sizeof(transform));
				svg_parse_transform(c, transform);
			}
			else
			{
				double a = c->a, b = c->b, cc = c->cc, d = c->d, e = c->e, f = c->f;
				char transform[128];
				svg_attr_text(c, i, "transform", transform, sizeof(transform));
				svg_parse_transform(c, transform);
				svg_render_tag(c, i);
				c->a = a; c->b = b; c->cc = cc; c->d = d; c->e = e; c->f = f;
			}
		}
		else if (token->type == SXML_ENDTAG && group_depth)
		{
			const char *name = c->buffer + token->startpos;
			if (svg_eq(name, token->endpos - token->startpos, "g"))
			{
				group_depth--;
				c->a = saved_a[group_depth];
				c->b = saved_b[group_depth];
				c->cc = saved_c[group_depth];
				c->d = saved_d[group_depth];
				c->e = saved_e[group_depth];
				c->f = saved_f[group_depth];
			}
		}
	}
}

SVG_FUNC void DISPLAY_FUNC(draw_svg_buff)(int x, int y, int w, int h, const char *buffer, size_t buffer_len, uint32_t override_color)
{
	if (!buffer || !w || !h || buffer_len > 0xffffffffU)
		return;
	unsigned token_capacity = SVG_TOKEN_COUNT;
	unsigned maximum_tokens = (unsigned)buffer_len + 1;
	sxmltok_t *tokens = NULL;
	sxml_t parser;
	sxmlerr_t err;
	if (token_capacity > maximum_tokens)
		token_capacity = maximum_tokens;
	for (;;)
	{
		tokens = (sxmltok_t *)malloc(token_capacity * sizeof(*tokens));
		if (!tokens)
			return;
		sxml_init(&parser);
		err = sxml_parse(&parser, buffer, (unsigned)buffer_len, tokens, token_capacity);
		if (err == SXML_SUCCESS)
			break;
		free(tokens);
		tokens = NULL;
		if (err != SXML_ERROR_TOKENSFULL || token_capacity >= maximum_tokens)
			return;
		token_capacity *= 2;
		if (token_capacity > maximum_tokens)
			token_capacity = maximum_tokens;
	}
	t_svg_ctx c;
#ifdef __cplusplus
	c.display = this;
#endif
	c.buffer = buffer;
	c.tokens = tokens;
	c.count = parser.ntokens;
	c.x = x;
	c.y = y;
	c.w = w;
	c.h = h;
	c.vx = 0;
	c.vy = 0;
	c.vw = 100;
	c.vh = 100;
	c.a = 1;
	c.b = 0;
	c.cc = 0;
	c.d = 1;
	c.e = 0;
	c.f = 0;
	c.override_color = override_color;
	for (unsigned i = 0; i < c.count; i++)
		if (tokens[i].type == SXML_STARTTAG && svg_eq(buffer + tokens[i].startpos, tokens[i].endpos - tokens[i].startpos, "svg"))
		{
			double v;
			if (svg_attr(&c, i, "width", &v) && v > 0)
				c.vw = v;
			if (svg_attr(&c, i, "height", &v) && v > 0)
				c.vh = v;
			char vb[128];
			svg_attr_text(&c, i, "viewBox", vb, sizeof(vb));
			if (vb[0])
			{
				char *p = vb;
				c.vx = svg_cnumber(p);
				while (*p && *p != ' ' && *p != ',')
					p++;
				while (*p == ' ' || *p == ',')
					p++;
				c.vy = svg_cnumber(p);
				while (*p && *p != ' ' && *p != ',')
					p++;
				while (*p == ' ' || *p == ',')
					p++;
				c.vw = svg_cnumber(p);
				while (*p && *p != ' ' && *p != ',')
					p++;
				while (*p == ' ' || *p == ',')
					p++;
				c.vh = svg_cnumber(p);
			}
			break;
		}
	if (c.vw <= 0)
		c.vw = 100;
	if (c.vh <= 0)
		c.vh = 100;
	svg_draw_tree(&c, 0, c.count);
	free(tokens);
}

SVG_FUNC int DISPLAY_FUNC(draw_svg)(int x, int y, int w, int h, const char *path, uint32_t override_color)
{
	if (!path || w <= 0 || h <= 0)
		return 1;
	int fd = open(path, O_READ);
	if (fd < 0)
		return 1;
	char *svg = (char *)malloc(SVG_BUFFER_LIMIT + 1);
	if (!svg)
	{
		close(fd);
		return 1;
	}
	int total = 0, amount;
	while (total < SVG_BUFFER_LIMIT && (amount = read(fd, svg + total, SVG_BUFFER_LIMIT - total)) > 0)
		total += amount;
	close(fd);
	if (!total)
	{
		free(svg);
		return 1;
	}
	svg[total] = 0;
	draw_svg_buff(x, y, w, h, svg, total, override_color);
	free(svg);
	return 0;
}

#endif
