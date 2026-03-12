#include "uart.h"

#define UART0 ((volatile unsigned int*)0x09000000)

void uart_putc(char c)
{
    *UART0 = c;
}

void uart_print(const char* str)
{
    while(*str)
        uart_putc(*str++);
}