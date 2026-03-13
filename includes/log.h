#ifndef LOG_H
#define LOG_H

#include "types.h"

typedef enum
{
	LOG_SUCCESS,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_NONE
}	e_logtype;

typedef enum
{
	BLACK,
	DARK_RED,
	RED,
	ORANGE,
	YELLOW,
	GREEN,
	LIME,
	DARK_BLUE,
	BLUE,
	DARK_CYAN,
	CYAN,
	DARK_MAGENTA,
	MAGENTA,
	DARK_GREY,
	GREY,
	WHITE
}	e_colors;

extern const char *COLORS[16];

void log(const char *fmt, e_logtype type, ...);

#endif