#include "log.h"
#include "random.h"
#include "mutex.h"
#include <stdarg.h>

const char *UART_COLORS[17] = {"\e[0;38;5;0;49m", "\e[0;38;5;0;49m", "\e[0;31m", "\e[1;31m", "\e[38;5;208m", "\e[1;33m", "\e[0;32m", "\e[1;32m", "\e[0;34m", "\e[1;34m", "\e[0;36m", "\e[1;36m", "\e[0;35m", "\e[1;35m", "\e[0;30m", "\e[1;30m", "\e[1;37m"};

int		ft_vprintf(const char *format, va_list *args);

static t_mutex	log_mutex = MUTEX_INITIALIZER;

void	log(const char *fmt, e_logtype type, ...)
{
	va_list args;
	va_start(args, type);
	const char *color = UART_COLORS[WHITE];
	const char *message = "      ";
	int indent = 0;

	mutex_lock(&log_mutex);
	rng_add_entropy(time_us() ^ type);
	
	if ((type & 16) == 16)
	{
		indent = 1;
		type &= ~16;
	}

	switch (type)
	{
		case LOG_ERROR:   color = UART_COLORS[RED]; message = " FAIL "; break;
		case LOG_WARNING: color = UART_COLORS[YELLOW]; message = " WARN "; break;
		case LOG_SUCCESS: color = UART_COLORS[GREEN]; message = " GOOD "; break;
		case LOG_INFO:    color = UART_COLORS[CYAN]; message = " INFO "; break;
		default: break;
	}
	if (type != LOG_NONE)
	{
		if (indent == 1) // INDENT TYPE
			uart_print("      =>");
		else
		{
			uart_print(UART_COLORS[WHITE]);
			uart_print("[");
		}
		uart_print(color);
		uart_print(message);
		uart_print(UART_COLORS[WHITE]);
		if (indent == 0)
			uart_print("] ");
	}
	//uart_print(fmt);
	ft_vprintf(fmt, &args);
	uart_print("\033[0m"); // reset color
	va_end(args);

	mutex_unlock(&log_mutex);
}
