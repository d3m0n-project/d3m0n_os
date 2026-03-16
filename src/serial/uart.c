#include "uart.h"
#include "types.h"
#include "math.h"

#define UART0 ((volatile unsigned int*)0x3F201000)

void uart_putc(char c)
{
    *UART0 = c;
}

void uart_print(const char* str)
{
    while(*str)
        uart_putc(*str++);
}

void    uart_putnbr_i(int nb)
{
    if (nb == 0)
    {
        uart_putc('0');
        return;
    }
    if (nb < 0)
        uart_putc('-');
    uart_putnbr_llu((unsigned long long)ABS(nb));
}

void    uart_putnbr_llu(unsigned long long nb)
{
    if (nb == 0)
        uart_putc('0');
    else if (nb > 0)
    {
        if (nb / 10 > 0)
            uart_putnbr_llu(nb / 10);
        uart_putc('0' + (nb % 10));
    }
}

void    uart_putnbr_llu_hex(unsigned long long nb)
{
    char    set[17] = "0123456789ABCDEF";
    if (nb == 0)
        uart_putc('0');
    else if (nb > 0)
    {
        if (nb / 16 > 0)
            uart_putnbr_llu_hex(nb / 16);
        uart_putc(set[nb % 16]);
    }
}