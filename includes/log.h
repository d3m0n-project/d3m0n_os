#ifndef LOG_H
#define LOG_H

#include "types.h"

typedef enum
{
	LOG_NONE=0,
	LOG_SUCCESS=1,
	LOG_ERROR=2,
	LOG_WARNING=4,
	LOG_INFO=8,
	LOG_INDENT=16
}	e_logtype;

extern const char *UART_COLORS[16];

void log(const char *fmt, e_logtype type, ...);

#endif