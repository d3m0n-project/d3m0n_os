#include "log.h"
#include "uart.h"

const char *COLORS[20] = {"\e[0m", "\e[0;30m", "\e[1;31m", "\e[1;32m", "\e[1;33m", "\e[1;34m", "\e[1;35m", "\e[1;36m", "\e[1;37m", "\e[38;5;208m", "\e[0;31m", "\e[1;38;5;208m", "\e[0;33m", "\e[0;32m", "\e[0;34m", "\e[0;36m", "\e[0;35m", "\e[1;30m", "\e[0;30m"};

void	log(const char *str, e_logtype type)
{
	#if DEBUG == 1
		char	*color = (char *)COLORS[WHITE];
		char	*message = "      ";
		switch (type)
		{
		case LOG_ERROR:
			color = (char *)COLORS[RED];
			message = " FAIL ";
			break;
		case LOG_WARNING:
			color = (char *)COLORS[YELLOW];
			message = " WARN ";
			break;
		case LOG_SUCCESS:
			color = (char *)COLORS[GREEN];
			message = "  OK  ";
			break;
		case LOG_INFO:
			color = (char *)COLORS[CYAN];
			message = " INFO ";
			break;
		case LOG_NONE:
			uart_print(str);
			return;
		}
		uart_putc('[');
		uart_print(color);
		uart_print(message);
		uart_print(COLORS[BLACK]);
		uart_print("] ");
		uart_print(str);
	#endif
}