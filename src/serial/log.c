#include "log.h"
#include "uart.h"
#include "color.h"

const char *UART_COLORS[16] = {"\e[0;38;5;0;49m", "\e[0;31m", "\e[1;31m", "\e[38;5;208m", "\e[1;33m", "\e[0;32m", "\e[1;32m", "\e[0;34m", "\e[1;34m", "\e[0;36m", "\e[1;36m", "\e[0;35m", "\e[1;35m", "\e[0;30m", "\e[1;30m", "\e[1;37m"};

void    log(const char *fmt, e_logtype type, ...)
{
#if DEBUG == 1
    va_list args;
    va_start(args, type);
    const char *color = UART_COLORS[WHITE];
    const char *message = "      ";

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
        uart_putc('[');
        uart_print(color);
        uart_print(message);
        uart_print(UART_COLORS[WHITE]);
        uart_print("] ");
    }
    for (int i = 0; fmt[i]; i++)
    {
        if (fmt[i] == '%' && fmt[i+1])
        {
            i++;

            if (fmt[i] == 's')
                uart_print(va_arg(args, char*));
            else if (fmt[i] == 'i' || fmt[i] == 'd')
                uart_putnbr_i(va_arg(args, int));
            else if (fmt[i] == 'l' && fmt[i++ + 1] == 'l' && fmt[i++ + 1] == 'u')
                uart_putnbr_llu(va_arg(args, unsigned long long));
            else
                uart_putc(fmt[i]);
        }
        else
            uart_putc(fmt[i]);
    }
    va_end(args);
#endif
}