#ifndef UART_H
#define UART_H

void	uart_putc(char c);
void	uart_print(const char* str);
void    uart_putnbr_i(int nb);
void    uart_putnbr_u(unsigned int nb);
void    uart_putnbr_llu(unsigned long long nb);
void    uart_putnbr_llu_hex(unsigned long long nb);

#endif