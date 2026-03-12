#ifndef LOG_H
#define LOG_H

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

extern const char *COLORS[20];

void	log(const char *str, e_logtype type);

#endif