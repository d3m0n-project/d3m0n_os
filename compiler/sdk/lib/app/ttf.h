#ifndef TTF_H
#define TTF_H

#include "types.h"

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
} 	t_font;

typedef void (*ttf_pixel_fn)(void *context, int x, int y, uint8_t coverage);

static inline int		ttf_parse(t_font *font);
static inline uint16_t	ttf_glyph_for_codepoint(const t_font *font, uint32_t codepoint);
static inline int		ttf_glyph_advance(const t_font *font, uint16_t glyph);
static inline void		ttf_render_glyph(const t_font *font, uint16_t glyph, int origin_x, int baseline_y, int scale_x, int scale_y, ttf_pixel_fn pixel, void *context);

#ifndef TTF_IMPLEMENTATION
#define TTF_IMPLEMENTATION

static uint16_t ttf_u16(const uint8_t *p) {
	return ((uint16_t)p[0] << 8) | p[1];
}
static int16_t ttf_i16(const uint8_t *p) {
	return (int16_t)ttf_u16(p);
}
static uint32_t ttf_u32(const uint8_t *p) {
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}
static int ttf_range(const t_font *f, uint32_t at, uint32_t n) {
	return at <= f->size && n <= f->size - at;
}
static int ttf_glyph_range(uint32_t at, uint32_t n, uint32_t size) {
	return at <= size && n <= size - at;
}
static uint16_t ttf_gu16(const uint8_t *g, uint32_t at)
{
    const volatile uint8_t *p = g + at;
    return ((uint16_t)p[0] << 8) | p[1];
}
static int16_t ttf_gi16(const uint8_t *g, uint32_t at)
{
    return (int16_t)ttf_gu16(g, at);
}

static uint32_t ttf_table(const t_font *f, uint32_t tag)
{
	uint16_t count;
	uint32_t p;
	if (!ttf_range(f, 0, 12) || ttf_u32(f->data) != 0x00010000U)
		return 0;
	count = ttf_u16(f->data + 4);
	if (!ttf_range(f, 12, (uint32_t)count * 16)) return 0;
	for (uint16_t i = 0; i < count; i++)
	{
		p = 12 + (uint32_t)i * 16;
		if (ttf_u32(f->data + p) == tag)
		{
			p = ttf_u32(f->data + p + 8);
			return ttf_range(f, p, ttf_u32(f->data + 12 + (uint32_t)i * 16 + 12)) ? p : 0;
		}
	}
	return 0;
}

static inline int ttf_parse(t_font *f)
{
	uint32_t cmap_table, p;
	uint16_t count;
	if (!f || !f->data || f->size < 12)
		return 1;
	f->head = ttf_table(f, 0x68656164); f->hhea = ttf_table(f, 0x68686561);
	f->hmtx = ttf_table(f, 0x686d7478); f->maxp = ttf_table(f, 0x6d617870);
	f->loca = ttf_table(f, 0x6c6f6361); f->glyf = ttf_table(f, 0x676c7966);
	cmap_table = ttf_table(f, 0x636d6170);
	if (!f->head || !f->hhea || !f->hmtx || !f->maxp || !f->loca || !f->glyf || !cmap_table)
		return 1;
	if (!ttf_range(f, f->head, 54) || !ttf_range(f, f->hhea, 36) || !ttf_range(f, f->maxp, 6))
		return 1;
	f->units_per_em = ttf_u16(f->data + f->head + 18);
	f->index_to_loc_format = ttf_i16(f->data + f->head + 50);
	f->ascender = ttf_i16(f->data + f->hhea + 4);
	f->descender = ttf_i16(f->data + f->hhea + 6);
	f->num_hmetrics = ttf_u16(f->data + f->hhea + 34);
	f->num_glyphs = ttf_u16(f->data + f->maxp + 4);
	if (!f->units_per_em || !f->num_glyphs || !f->num_hmetrics || f->num_hmetrics > f->num_glyphs)
		return 1;
	if (f->index_to_loc_format != 0 && f->index_to_loc_format != 1)
		return 1;
	if (!ttf_range(f, cmap_table, 4))
		return 1;
	count = ttf_u16(f->data + cmap_table + 2);
	if (!ttf_range(f, cmap_table, 4 + (uint32_t)count * 8))
		return 1;
	for (uint16_t i = 0; i < count; i++)
	{
		p = cmap_table + 4 + (uint32_t)i * 8;
		uint16_t platform = ttf_u16(f->data + p), encoding = ttf_u16(f->data + p + 2);
		uint32_t sub = cmap_table + ttf_u32(f->data + p + 4);
		if ((platform == 3 && (encoding == 1 || encoding == 10)) || platform == 0)
		{
			if (ttf_range(f, sub, 2) && ttf_u16(f->data + sub) == 4)
			{
				f->cmap = sub;
				break;
			}
		}
	}
	return f->cmap ? 0 : 1;
}

static inline uint16_t ttf_glyph_for_codepoint(const t_font *f, uint32_t cp)
{
	const uint8_t *p; uint16_t segs, end, start, ro; int16_t delta;
	if (!f || !f->cmap || cp > 0xffff || !ttf_range(f, f->cmap, 16))
		return 0;
	p = f->data + f->cmap; segs = ttf_u16(p + 6) / 2;
	if (!ttf_range(f, f->cmap, 16 + (uint32_t)segs * 8))
		return 0;
	for (uint16_t i = 0; i < segs; i++)
	{
		end = ttf_u16(p + 14 + i * 2);
		if (cp > end)
			continue;
		start = ttf_u16(p + 16 + segs * 2 + i * 2);
		if (cp < start)
			return 0;
		delta = ttf_i16(p + 16 + segs * 4 + i * 2);
		ro = ttf_u16(p + 16 + segs * 6 + i * 2);
		if (!ro)
			return (uint16_t)(cp + delta);
		uint32_t at = (uint32_t)(p + 16 + segs * 6 + i * 2 - f->data) + ro + (cp - start) * 2;
		if (!ttf_range(f, at, 2))
			return 0;
		return (uint16_t)(ttf_u16(f->data + at) ? ttf_u16(f->data + at) + delta : 0);
	}
	return 0;
}

static inline int ttf_glyph_advance(const t_font *f, uint16_t g)
{
	uint32_t at; uint16_t index;
	if (!f || g >= f->num_glyphs)
		return 0;
	index = g < f->num_hmetrics ? g : f->num_hmetrics - 1;
	at = f->hmtx + (uint32_t)index * 4;
	return ttf_range(f, at, 2) ? ttf_u16(f->data + at) : 0;
}


static inline void ttf_render_glyph(const t_font *f, uint16_t glyph, int ox, int base, int sx, int sy, ttf_pixel_fn pixel, void *ctx)
{
	typedef struct { int x, y; uint8_t on; } point;
	typedef struct { int x0, y0, x1, y1; } edge;
	static point pts[256];
	static edge edges[768];
	static uint16_t ends[64];
	static uint8_t flags[256];
	uint32_t lp, gp, at, glyph_size; int ne = 0; uint16_t contours; int16_t x = 0, y = 0;
	if (!f || !pixel || glyph >= f->num_glyphs || sx <= 0 || sy <= 0)
		return;
	at = f->loca + (f->index_to_loc_format ? (uint32_t)glyph * 4 : (uint32_t)glyph * 2);
	if (!ttf_range(f, at, f->index_to_loc_format ? 8 : 4))
		return;
	lp = f->index_to_loc_format ? ttf_u32(f->data + at) : (uint32_t)ttf_u16(f->data + at) * 2;
	gp = f->index_to_loc_format ? ttf_u32(f->data + at + 4) : (uint32_t)ttf_u16(f->data + at + 2) * 2;
	if (gp <= lp || !ttf_range(f, f->glyf + lp, gp - lp) || gp - lp < 10)
		return;
	glyph_size = gp - lp;

	const uint8_t *g = f->data + f->glyf + lp;
	contours = ttf_gu16(g, 0);
	if ((int16_t)contours <= 0 || contours > 64 || !ttf_glyph_range(10, (uint32_t)contours * 2, glyph_size))
		return;
	at = 10 + contours * 2;
	if (!ttf_glyph_range(at, 2, glyph_size))
		return;
	for (uint16_t i=0;i<contours;i++)
		ends[i]=ttf_gu16(g, 10 + i * 2);
	uint16_t total = ends[contours - 1] + 1;
	if (total > 256)
		return;
	uint16_t ins=ttf_gu16(g, at);
	at += 2;
	if (!ttf_glyph_range(at, ins, glyph_size))
		return;
	at += ins;
	if (!ttf_glyph_range(at, 1, glyph_size))
		return;
	uint16_t fi=0;
	while (fi < total && at < glyph_size)
	{
		uint8_t q=g[at++];
		flags[fi++]=q;
		if (q&8)
		{
			if(!ttf_glyph_range(at, 1, glyph_size))
				return;
			uint8_t rep=g[at++];
			while(rep-- && fi<total)
				flags[fi++]=q;
		}
	}
	if (fi != total)
		return;

	for (uint16_t i=0;i<total;i++)
	{
		int d=0;
		if(flags[i]&2)
		{
			if(!ttf_glyph_range(at, 1, glyph_size))
				return;
			d=g[at++];
			if(!(flags[i]&16))
				d=-d;
		}
		else if(flags[i]&16)
			d=0;
		else
		{
			if(!ttf_glyph_range(at, 2, glyph_size))
				return;
			d=ttf_gi16(g, at);
			at+=2;
		}
		x += d;
		pts[i].x=x;
		pts[i].on=flags[i]&1;
	}
	
	x=0;
	for(uint16_t i=0;i<total;i++)
	{
		int d=0;
		if(flags[i]&4)
		{
			if(!ttf_glyph_range(at, 1, glyph_size))
				return;
			d=g[at++];
			if(!(flags[i]&32))
				d=-d;
		}
		else if(flags[i]&32)
			d=0;
		else
		{
			if(!ttf_glyph_range(at, 2, glyph_size))
				return;
			d=ttf_gi16(g, at);
			at+=2;
		}
		y+=d;
		pts[i].y=y;
	}
	uint16_t first=0;
	for(uint16_t ci=0;ci<contours;ci++)
	{
		uint16_t last=ends[ci], count=last-first+1, j;
		if(!count)
		{
			first=last+1;
			continue;
		}
		point a=pts[last], b=pts[first];
		point cur;
		if(!a.on&&!b.on)
		{
			cur.x=(a.x+b.x)/2;
			cur.y=(a.y+b.y)/2;
			cur.on=1;
		}
		else cur=a.on?a:b;
		
		for(j=0;j<count;j++)
		{
			point q=pts[first+j], r=pts[first+(j+1)%count];
			if(q.on&&r.on)
			{
				if(ne<768)
				{
					edges[ne++]=(edge){cur.x,cur.y,q.x,q.y};
				}
				cur=q;
			}
			else if(!q.on)
			{
				point z=r.on?r:(point){(q.x+r.x)/2,(q.y+r.y)/2,1};
				for(int k=1;k<=8;k++)
				{
					int px=q.x+(z.x-q.x)*k/8,py=q.y+(z.y-q.y)*k/8;
					if(ne<768)
						edges[ne++]=(edge){cur.x,cur.y,px,py};
					cur.x=px;cur.y=py;
				}
				if(r.on)
					cur=r;
			}
			else
				cur=q;
		}
		first=last+1;
	}
	if (!ne)
		return;
	
	int minx=0,maxx=0,miny=0,maxy=0;
	for(int i=0;i<ne;i++)
	{
		if(i==0||edges[i].x0<minx)
			minx=edges[i].x0;
		if(i==0||edges[i].x1<minx)
			minx=edges[i].x1;
		if(i==0||edges[i].x0>maxx)
			maxx=edges[i].x0;
		if(i==0||edges[i].x1>maxx)
			maxx=edges[i].x1;
		if(i==0||edges[i].y0<miny)
			miny=edges[i].y0;
		if(i==0||edges[i].y1<miny)
			miny=edges[i].y1;
		if(i==0||edges[i].y0>maxy)
			maxy=edges[i].y0;
		if(i==0||edges[i].y1>maxy)
			maxy=edges[i].y1;
	}
	for(int py=(base-maxy*sy/f->units_per_em)-1;py<base-miny*sy/f->units_per_em+2;py++)
	{
		for(int px=ox+minx*sx/f->units_per_em-1;px<ox+maxx*sx/f->units_per_em+2;px++)
		{
			int winding=0;
			int qx=(px-ox)*f->units_per_em/sx,qy=(base-py)*f->units_per_em/sy;
			for(int i=0;i<ne;i++)
			{
				edge e=edges[i];
				if((e.y0<=qy&&e.y1>qy)||(e.y1<=qy&&e.y0>qy))
				{
					int ix=e.x0+(e.x1-e.x0)*(qy-e.y0)/(e.y1-e.y0);
					if(ix>qx)winding+=(e.y1>e.y0)?1:-1;
				}
			}
			if(winding)
				pixel(ctx,px,py,255);
		}
	}
}

#endif

#endif
