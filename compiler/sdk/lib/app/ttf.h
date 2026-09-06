#ifndef TTF_H
#define TTF_H

#include "types.h"
//#include "stdio.hpp"

typedef struct s_font
{
	uint8_t		*data;
	uint32_t	size;
	uint16_t	units_per_em;
	uint16_t	num_glyphs;
	uint16_t	num_hmetrics;
	int16_t		index_to_loc_format;
	int16_t		ascender;
	int16_t		descender;
	uint32_t	head;
	uint32_t	hhea;
	uint32_t	hmtx;
	uint32_t	maxp;
	uint32_t	loca;
	uint32_t	glyf;
	uint32_t	cmap;
	char		name[25];
}	t_font;

typedef void			(*ttf_pixel_fn)(void *context, int x, int y, uint8_t coverage);
static inline int		ttf_parse(t_font *font);
static inline uint16_t	ttf_glyph_for_codepoint(const t_font *font, uint32_t codepoint);
static inline int		ttf_glyph_advance(const t_font *font, uint16_t glyph);
static inline void		ttf_render_glyph(const t_font *font, uint16_t glyph, int origin_x, int baseline_y, int scale_x, int scale_y, ttf_pixel_fn pixel, void *context);

#ifndef TTF_IMPLEMENTATION
#define TTF_IMPLEMENTATION

static uint16_t	ttf_u16(const uint8_t *p)
{
	return ((uint16_t)p[0] << 8) | p[1];
}

static int16_t	ttf_i16(const uint8_t *p)
{
	return (int16_t)ttf_u16(p);
}

static uint32_t	ttf_u32(const uint8_t *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static int	ttf_range(const t_font *f, uint32_t at, uint32_t n)
{
	return (at <= f->size && n <= f->size - at);
}

static int	ttf_glyph_range(uint32_t at, uint32_t n, uint32_t size)
{
	return (at <= size && n <= size - at);
}

static uint16_t	ttf_gu16(const uint8_t *g, uint32_t at)
{
	return ((uint16_t)g[at] << 8) | g[at + 1];
}

static int16_t	ttf_gi16(const uint8_t *g, uint32_t at)
{
	return (int16_t)ttf_gu16(g, at);
}

static uint32_t	ttf_table(const t_font *f, uint32_t tag)
{
	uint16_t	count;
	uint32_t	p;

	if (!ttf_range(f, 0, 12) || ttf_u32(f->data) != 0x00010000U)
		return (0);

	count = ttf_u16(f->data + 4);
	if (!ttf_range(f, 12, (uint32_t)count * 16))
		return (0);

	for (uint16_t i = 0; i < count; i++)
	{
		p = 12 + (uint32_t)i * 16;
		if (ttf_u32(f->data + p) == tag)
		{
			p = ttf_u32(f->data + p + 8);
			return (ttf_range(f, p, ttf_u32(f->data + 12 + (uint32_t)i * 16 + 12)) ? p : 0);
		}
	}
	return (0);
}

static inline int	ttf_parse(t_font *f)
{
	uint32_t	cmap_table;
	uint32_t	p;
	uint16_t	count;
	uint16_t	i;
	uint16_t	platform;
	uint16_t	encoding;
	uint32_t	sub;

	if (!f || !f->data || f->size < 12)
		return (1);
	
	f->head = ttf_table(f, 0x68656164);
	f->hhea = ttf_table(f, 0x68686561);
	f->hmtx = ttf_table(f, 0x686d7478);
	f->maxp = ttf_table(f, 0x6d617870);
	f->loca = ttf_table(f, 0x6c6f6361);
	f->glyf = ttf_table(f, 0x676c7966);
	cmap_table = ttf_table(f, 0x636d6170);
	if (!f->head || !f->hhea || !f->hmtx || !f->maxp || !f->loca || !f->glyf || !cmap_table)
		return (1);
	if (!ttf_range(f, f->head, 54) || !ttf_range(f, f->hhea, 36) || !ttf_range(f, f->maxp, 6))
		return (1);
	f->units_per_em = ttf_u16(f->data + f->head + 18);
	f->index_to_loc_format = ttf_i16(f->data + f->head + 50);
	f->ascender = ttf_i16(f->data + f->hhea + 4);
	f->descender = ttf_i16(f->data + f->hhea + 6);
	f->num_hmetrics = ttf_u16(f->data + f->hhea + 34);
	f->num_glyphs = ttf_u16(f->data + f->maxp + 4);
	if (!f->units_per_em || !f->num_glyphs || !f->num_hmetrics || f->num_hmetrics > f->num_glyphs)
		return (1);
	if (f->index_to_loc_format != 0 && f->index_to_loc_format != 1)
		return (1);
	if (!ttf_range(f, cmap_table, 4))
		return (1);
	count = ttf_u16(f->data + cmap_table + 2);
	if (!ttf_range(f, cmap_table, 4 + (uint32_t)count * 8))
		return (1);
	i = 0;
	f->cmap = 0;
	while (i < count)
	{
		p = cmap_table + 4 + (uint32_t)i * 8;
		platform = ttf_u16(f->data + p);
		encoding = ttf_u16(f->data + p + 2);
		sub = cmap_table + ttf_u32(f->data + p + 4);
		if ((platform == 3 && (encoding == 1 || encoding == 10)) || platform == 0)
		{
			if (ttf_range(f, sub, 2) && ttf_u16(f->data + sub) == 4)
			{
				f->cmap = sub;
				break ;
			}
		}
		i++;
	}
	return (f->cmap?0:1);
}

static inline uint16_t	ttf_glyph_for_codepoint(const t_font *f, uint32_t cp)
{
	const uint8_t	*p;
	uint16_t		segs;
	uint16_t		end;
	uint16_t		start;
	uint16_t		ro;
	int16_t			delta;
	uint32_t		at;
	uint16_t		glyph;

	if (!f || !f->cmap || cp > 0xffff || !ttf_range(f, f->cmap, 16))
		return 0;
	p = f->data + f->cmap;
	segs = ttf_u16(p + 6) / 2;
	if (!ttf_range(f, f->cmap, 16 + (uint32_t)segs * 8))
		return 0;
	end = 0;
	start = 0;
	delta = 0;
	ro = 0;
	for (uint16_t i = 0; i < segs; i++)
	{
		end = ttf_u16(p + 14 + i * 2);
		if (cp > end)
			continue ;
		start = ttf_u16(p + 16 + segs * 2 + i * 2);
		if (cp < start)
			return 0;
		delta = ttf_i16(p + 16 + segs * 4 + i * 2);
		ro = ttf_u16(p + 16 + segs * 6 + i * 2);
		if (!ro)
			return ((uint16_t)(cp + delta));
		at = (uint32_t)(p + 16 + segs * 6 + i * 2 - f->data);
		at += ro + (cp - start) * 2;
		if (!ttf_range(f, at, 2))
			return 0;
		glyph = ttf_u16(f->data + at);
		if (glyph)
			glyph = (uint16_t)(glyph + delta);
		return (glyph);
	}
	return 0;
}

static inline int	ttf_glyph_advance(const t_font *f, uint16_t g)
{
	uint32_t	at;
	uint16_t	index;

	if (!f || g >= f->num_glyphs || !f->num_hmetrics)
		return 0;
	if (g < f->num_hmetrics)
		index = g;
	else
		index = f->num_hmetrics - 1;
	at = f->hmtx + (uint32_t)index * 4;
	if (!ttf_range(f, at, 2))
		return 0;
	return ((int)ttf_u16(f->data + at));
}

static inline void	ttf_render_glyph_depth(const t_font *f, uint16_t glyph, int ox, int base, int sx, int sy, ttf_pixel_fn pixel, void *ctx, int depth)
{
	typedef struct s_point
	{
		int			x;
		int			y;
		uint8_t		on;
	}	t_point;

	typedef struct s_edge
	{
		int			x0;
		int			y0;
		int			x1;
		int			y1;
	}	t_edge;

	static t_point	pts[256];
	static t_edge	edges[1024];
	static uint16_t	ends[64];
	static uint8_t	flags[256];

	uint32_t	lp;
	uint32_t	gp;
	uint32_t	at;
	uint32_t	glyph_size;
	uint16_t	contours;
	uint16_t	total;
	uint16_t	ins;
	uint16_t	fi;
	uint16_t	first;
	int			ne;
	int			x;
	int			y;

	if (!f || !pixel || glyph >= f->num_glyphs || sx <= 0 || sy <= 0 || !f->units_per_em || depth > 16)
		return ;

	at = f->loca + (f->index_to_loc_format?((uint32_t)glyph * 4):((uint32_t)glyph * 2));
	if (!ttf_range(f, at, f->index_to_loc_format ? 8 : 4))
		return ;

	if (f->index_to_loc_format)
	{
		lp = ttf_u32(f->data + at);
		gp = ttf_u32(f->data + at + 4);
	}
	else
	{
		lp = (uint32_t)ttf_u16(f->data + at) * 2;
		gp = (uint32_t)ttf_u16(f->data + at + 2) * 2;
	}

	if (gp <= lp || !ttf_range(f, f->glyf + lp, gp - lp))
		return ;

	glyph_size = gp - lp;
	if (glyph_size < 10)
		return ;


	const uint8_t *g = f->data + f->glyf + lp;
	contours = ttf_gu16(g, 0);
	//printf("glyph=%u lp=%u gp=%u size=%u contours=%d\n",
		//glyph, lp, gp, glyph_size, (int16_t)contours);
	if ((int16_t)contours < 0)
	{
		uint32_t	component_at;
		uint16_t	flags;
		int		component_count;

		component_at = 10;
		component_count = 0;
		do
		{
			uint16_t	component_glyph;
			int		arg_x;
			int		arg_y;
			int		component_sx;
			int		component_sy;

			if (++component_count > 32 || !ttf_glyph_range(component_at, 4, glyph_size))
				return ;
			flags = ttf_gu16(g, component_at);
			component_glyph = ttf_gu16(g, component_at + 2);
			component_at += 4;
			if (flags & 1)
			{
				if (!ttf_glyph_range(component_at, 4, glyph_size))
					return ;
				arg_x = ttf_gi16(g, component_at);
				arg_y = ttf_gi16(g, component_at + 2);
				component_at += 4;
			}
			else
			{
				if (!ttf_glyph_range(component_at, 2, glyph_size))
					return ;
				arg_x = (int8_t)g[component_at];
				arg_y = (int8_t)g[component_at + 1];
				component_at += 2;
			}

			component_sx = sx;
			component_sy = sy;
			if (flags & 8)
			{
				int scale;
				if (!ttf_glyph_range(component_at, 2, glyph_size))
					return ;
				scale = ttf_gi16(g, component_at);
				component_sx = (int)((int64_t)sx * scale / 16384);
				component_sy = (int)((int64_t)sy * scale / 16384);
				component_at += 2;
			}
			else if (flags & 64)
			{
				int scale_x;
				int scale_y;
				if (!ttf_glyph_range(component_at, 4, glyph_size))
					return ;
				scale_x = ttf_gi16(g, component_at);
				scale_y = ttf_gi16(g, component_at + 2);
				component_sx = (int)((int64_t)sx * scale_x / 16384);
				component_sy = (int)((int64_t)sy * scale_y / 16384);
				component_at += 4;
			}
			else if (flags & 128)
			{
				int scale_x;
				int scale_y;
				if (!ttf_glyph_range(component_at, 8, glyph_size))
					return ;
				scale_x = ttf_gi16(g, component_at);
				scale_y = ttf_gi16(g, component_at + 6);
				component_sx = (int)((int64_t)sx * scale_x / 16384);
				component_sy = (int)((int64_t)sy * scale_y / 16384);
				component_at += 8;
			}
			if (component_sx <= 0 || component_sy <= 0)
				return ;
			ttf_render_glyph_depth(f, component_glyph,
				ox + (int)((int64_t)arg_x * sx / f->units_per_em),
				base - (int)((int64_t)arg_y * sy / f->units_per_em),
				component_sx, component_sy, pixel, ctx, depth + 1);
		}
		while (flags & 32);
		return ;
	}

	if (contours == 0 || contours > 64)
		return ;

	if (!ttf_glyph_range(10, (uint32_t)contours * 2, glyph_size))
		return ;

	for (uint16_t i = 0; i < contours; i++)
		ends[i] = ttf_gu16(g, 10 + i * 2);

	total = ends[contours - 1] + 1;
	if (total == 0 || total > 256)
		return ;

	at = 10 + contours * 2;
	if (!ttf_glyph_range(at, 2, glyph_size))
		return ;

	ins = ttf_gu16(g, at);
	at += 2;
	if (!ttf_glyph_range(at, ins, glyph_size))
		return ;

	at += ins;
	fi = 0;
	while (fi < total)
	{
		uint8_t	flag;
		uint8_t	repeat;
		if (!ttf_glyph_range(at, 1, glyph_size))
			return ;

		flag = g[at++];
		flags[fi++] = flag;
		if (flag & 8)
		{
			if (!ttf_glyph_range(at, 1, glyph_size))
				return ;

			repeat = g[at++];

			while (repeat > 0 && fi < total)
			{
				flags[fi++] = flag;
				repeat--;
			}
		}
	}


	x = 0;
	for (uint16_t i = 0; i < total; i++)
	{
		int	delta = 0;
		if (flags[i] & 2)
		{
			if (!ttf_glyph_range(at, 1, glyph_size))
				return ;
			delta = g[at++];
			if (!(flags[i] & 16))
				delta = -delta;
		}
		else if (!(flags[i] & 16))
		{
			if (!ttf_glyph_range(at, 2, glyph_size))
				return ;
			delta = ttf_gi16(g, at);
			at += 2;
		}
		x += delta;
		pts[i].x = x;
		pts[i].on = flags[i] & 1;
	}


	y = 0;
	for (uint16_t i = 0; i < total; i++)
	{
		int	delta = 0;
		if (flags[i] & 4)
		{
			if (!ttf_glyph_range(at, 1, glyph_size))
				return ;
			delta = g[at++];
			if (!(flags[i] & 32))
				delta = -delta;
		}
		else if (!(flags[i] & 32))
		{
			if (!ttf_glyph_range(at, 2, glyph_size))
				return ;
			delta = ttf_gi16(g, at);
			at += 2;
		}
		y += delta;
		pts[i].y = y;
	}

	ne = 0;
	first = 0;
	for (uint16_t ci = 0; ci < contours; ci++)
	{
		uint16_t	last;
		uint16_t	count;
		t_point		start;
		t_point		cur;
		uint16_t	i;
		last = ends[ci];
		count = last - first + 1;
		if (!count)
		{
			first = last + 1;
			continue ;
		}

		if (pts[first].on)
			start = pts[first];
		else if (pts[last].on)
			start = pts[last];
		else
		{
			start.x = (pts[first].x + pts[last].x) / 2;
			start.y = (pts[first].y + pts[last].y) / 2;
			start.on = 1;
		}

		cur = start;
		i = 0;
		while (i < count)
		{
			t_point	p;
			t_point	next;

			p = pts[first + i];

			if (p.on)
			{
				// straight line
				if (cur.x != p.x || cur.y != p.y)
				{
					if (ne < 1024)
					{
						edges[ne].x0 = cur.x;
						edges[ne].y0 = cur.y;
						edges[ne].x1 = p.x;
						edges[ne].y1 = p.y;
						ne++;
					}
				}
				cur = p;
				i++;
				continue ;
			}


			next = pts[first + ((i + 1) % count)];
			if (!next.on)
			{
				t_point	mid;
				mid.x = (p.x + next.x) / 2;
				mid.y = (p.y + next.y) / 2;
				mid.on = 1;
				next = mid;
			}
			else
				i++;

			{
				int	k;
				int	prev_x;
				int	prev_y;

				prev_x = cur.x;
				prev_y = cur.y;

				k = 1;
				while (k <= 8)
				{
					int64_t	t;
					int64_t	mt;
					int64_t	px;
					int64_t	py;

					t = k;
					mt = 8 - t;

					px = mt * mt * cur.x
						+ 2 * mt * t * p.x
						+ t * t * next.x;
					py = mt * mt * cur.y
						+ 2 * mt * t * p.y
						+ t * t * next.y;

					px /= 64;
					py /= 64;

					if (ne < 1024)
					{
						edges[ne].x0 = prev_x;
						edges[ne].y0 = prev_y;
						edges[ne].x1 = (int)px;
						edges[ne].y1 = (int)py;
						ne++;
					}

					prev_x = (int)px;
					prev_y = (int)py;
					k++;
				}

				cur = next;
			}

			i++;
		}

		// close contour
		if (cur.x != start.x || cur.y != start.y)
		{
			if (ne < 1024)
			{
				edges[ne].x0 = cur.x;
				edges[ne].y0 = cur.y;
				edges[ne].x1 = start.x;
				edges[ne].y1 = start.y;
				ne++;
			}
		}

		first = last + 1;
	}

	if (!ne)
		return ;

	{
		int	minx;
		int	maxx;
		int	miny;
		int	maxy;

		minx = edges[0].x0;
		maxx = edges[0].x0;
		miny = edges[0].y0;
		maxy = edges[0].y0;

		for (int i = 0; i < ne; i++)
		{
			if (edges[i].x0 < minx)
				minx = edges[i].x0;
			if (edges[i].x1 < minx)
				minx = edges[i].x1;
			if (edges[i].x0 > maxx)
				maxx = edges[i].x0;
			if (edges[i].x1 > maxx)
				maxx = edges[i].x1;
			if (edges[i].y0 < miny)
				miny = edges[i].y0;
			if (edges[i].y1 < miny)
				miny = edges[i].y1;
			if (edges[i].y0 > maxy)
				maxy = edges[i].y0;
			if (edges[i].y1 > maxy)
				maxy = edges[i].y1;
		}

		int	screen_min_x;
		int	screen_max_x;
		int	screen_min_y;
		int	screen_max_y;

		screen_min_x = ox + (int)(((int64_t)minx * sx) / f->units_per_em) - 1;
		screen_max_x = ox + (int)(((int64_t)maxx * sx) / f->units_per_em) + 1;

		screen_min_y = base - (int)(((int64_t)maxy * sy) / f->units_per_em) - 1;
		screen_max_y = base - (int)(((int64_t)miny * sy) / f->units_per_em) + 1;

		// 4x4 supersampling
		for (int py = screen_min_y; py <= screen_max_y; py++)
		{
			for (int px = screen_min_x; px <= screen_max_x; px++)
			{
				int	covered = 0;
				for (int sample_y = 0; sample_y < 4; sample_y++)
				{
					for (int sample_x = 0; sample_x < 4; sample_x++)
					{
						int64_t	qx;
						int64_t	qy;
						int		winding;

						qx = (
							(int64_t)(px - ox) * 8
							* f->units_per_em
							+ (2 * sample_x + 1)
							* f->units_per_em
						) / (8 * sx);

						qy = (
							(int64_t)(base - py) * 8
							* f->units_per_em
							+ (2 * sample_y + 1)
							* f->units_per_em
						) / (8 * sy);

						winding = 0;

						for (int ei = 0; ei < ne; ei++)
						{
							t_edge	e;

							e = edges[ei];

							if ((e.y0 <= qy && e.y1 > qy)
								|| (e.y1 <= qy && e.y0 > qy))
							{
								int64_t	ix;

								ix = e.x0
									+ (int64_t)(e.x1 - e.x0)
									* (qy - e.y0)
									/ (e.y1 - e.y0);

								if (ix > qx)
								{
									if (e.y1 > e.y0)
										winding++;
									else
										winding--;
								}
							}
						}

						if (winding != 0)
							covered++;
					}
				}

				if (covered)
				{
					uint8_t	coverage;

					coverage = (uint8_t)(covered * 255 / 16);
					pixel(ctx, px, py, coverage);
				}
			}
		}
	}
}

static inline void	ttf_render_glyph(const t_font *f, uint16_t glyph, int ox, int base, int sx, int sy, ttf_pixel_fn pixel, void *ctx)
{
	ttf_render_glyph_depth(f, glyph, ox, base, sx, sy, pixel, ctx, 0);
}

#endif
#endif