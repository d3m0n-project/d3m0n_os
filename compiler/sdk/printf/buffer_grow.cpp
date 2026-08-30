#include "printf/printf.hpp"
#include "stdlib.h"

extern "C" int	buf_grow(t_buf *b, size_t extra)
{
	size_t	new_cap;
	char	*tmp;

	if (b->len + extra + 1 <= b->cap)
		return 1;
	new_cap = b->cap ? b->cap : 64;
	while (new_cap < b->len + extra + 1)
		new_cap *= 2;
	tmp = (char *)realloc(b->data, new_cap);
	if (!tmp)
		return (0);
	b->data = tmp;
	b->cap = new_cap;
	return 1;
}

extern "C" int	buf_putc(t_buf *b, char c)
{
	if (!buf_grow(b, 1))
		return (0);
	b->data[b->len++] = c;
	b->data[b->len] = '\0';
	return (1);
}

extern "C" int	buf_write(t_buf *b, const char *s, size_t n)
{
	if (!buf_grow(b, n))
		return (0);
	memcpy(b->data + b->len, s, n);
	b->len += n;
	b->data[b->len] = '\0';
	return (1);
}

extern "C" int	buf_putstr(t_buf *b, const char *s)
{
	return (buf_write(b, s, strlen(s)));
}