#ifndef UART_H
#define UART_H

#define UART0 ((volatile unsigned int*)0x09000000)

void	uart_putc(char c);
void	uart_print(const char* str);
void    uart_putnbr_i(int nb);
void    uart_putnbr_llu(unsigned long long nb);

#endif