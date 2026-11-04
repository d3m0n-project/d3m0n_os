#ifndef LOG_H
#define LOG_H

#include "types.h"
#include "protocol/uart.h"
#include "display/color.h"

typedef enum : int
{
	LOG_NONE=0,
	LOG_SUCCESS=1,
	LOG_ERROR=2,
	LOG_WARNING=4,
	LOG_INFO=8,
	LOG_INDENT=16
}	e_logtype;

#define ANSI_COLOR_RAW(id)       ("\033[" #id "m")

#define ANSI_COLOR_RESET         ANSI_COLOR_RAW(0)

#define ANSI_COLOR_GREY			"\033[38;2;190;190;200m"
#define ANSI_COLOR_RED			"\033[38;2;255;160;170m"
#define ANSI_COLOR_GREEN		"\033[38;2;170;220;180m"
#define ANSI_COLOR_YELLOW		"\033[38;2;255;225;160m"
#define ANSI_COLOR_BLUE			"\033[38;2;165;195;255m"
#define ANSI_COLOR_MAGENTA		"\033[38;2;225;175;235m"
#define ANSI_COLOR_CYAN			"\033[38;2;160;225;230m"
#define ANSI_COLOR_WHITE		"\033[38;2;245;240;235m"


extern const char *UART_COLORS[17];

void	log(const char *fmt, int type, ...);

#endif