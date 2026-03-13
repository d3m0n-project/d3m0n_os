#include "log.h"
#include "uart.h"

const char *COLORS[20] = {"\e[0m", "\e[0;30m", "\e[1;31m", "\e[1;32m", "\e[1;33m", "\e[1;34m", "\e[1;35m", "\e[1;36m", "\e[1;37m", "\e[38;5;208m", "\e[0;31m", "\e[1;38;5;208m", "\e[0;33m", "\e[0;32m", "\e[0;34m", "\e[0;36m", "\e[0;35m", "\e[1;30m", "\e[0;30m"};

void log(const char *fmt, e_logtype type, const char *arg)
{
#if DEBUG == 1
    const char *color = COLORS[BLACK];
    const char *message = "      ";

    switch (type)
    {
        case LOG_ERROR:   color = COLORS[1]; message = " FAIL "; break;
        case LOG_WARNING: color = COLORS[3]; message = " WARN "; break;
        case LOG_SUCCESS: color = COLORS[2]; message = "  OK  "; break;
        case LOG_INFO:    color = COLORS[4]; message = " INFO "; break;
        default: break;
    }
    if (type != LOG_NONE)
    {
        uart_putc('[');
        uart_print(color);
        uart_print(message);
        uart_print(COLORS[BLACK]);
        uart_print("] ");
    }
    for (int i = 0; fmt[i]; i++)
    {
        if (fmt[i] == '%' && fmt[i+1] == 's')
        {
            if (arg) uart_print(arg);
            i++;
        }
        else
            uart_putc(fmt[i]);
    }
#endif
}