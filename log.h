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

extern const char *UART_COLORS[16];

void log(const char *fmt, e_logtype type, ...);

#endif