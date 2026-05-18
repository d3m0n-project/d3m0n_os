#include "uart.h"
#include "types.h"
#include "math.h"
#include "time.h"

#define MMIO_BASE 0x20000000

#define GPFSEL1       ((volatile unsigned int*)(MMIO_BASE + 0x200004))
#define GPPUD         ((volatile unsigned int*)(MMIO_BASE + 0x200094))
#define GPPUDCLK0     ((volatile unsigned int*)(MMIO_BASE + 0x200098))

// PL011 UART
#define UART0_DR      ((volatile unsigned int*)(MMIO_BASE + 0x201000))
#define UART0_FR      ((volatile unsigned int*)(MMIO_BASE + 0x201018))
#define UART0_IBRD    ((volatile unsigned int*)(MMIO_BASE + 0x201024))
#define UART0_FBRD    ((volatile unsigned int*)(MMIO_BASE + 0x201028))
#define UART0_LCRH    ((volatile unsigned int*)(MMIO_BASE + 0x20102C))
#define UART0_CR      ((volatile unsigned int*)(MMIO_BASE + 0x201030))
#define UART0_IMSC    ((volatile unsigned int*)(MMIO_BASE + 0x201038))
#define UART0_ICR     ((volatile unsigned int*)(MMIO_BASE + 0x201044))

static void delay(int count)
{
    while (count--)
        asm volatile("nop");
}

void uart_init(void)
{
    unsigned int r;

    // disable UART
    *UART0_CR = 0;

    // GPIO14/15 = ALT0
    r = *GPFSEL1;

    r &= ~((7 << 12) | (7 << 15));
    r |=  (4 << 12) | (4 << 15);

    *GPFSEL1 = r;

    // disable pullup/down
    *GPPUD = 0;
    delay(1500);

    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    delay(1500);

    *GPPUDCLK0 = 0;

    // clear interrupts
    *UART0_ICR = 0x7FF;

    // baud rate 115200
    *UART0_IBRD = 1;
    *UART0_FBRD = 40;

    // 8n1
    *UART0_LCRH = (1 << 4) | (1 << 5) | (1 << 6);

    // mask interrupts
    *UART0_IMSC = 0;

    // enable uart tx rx
    *UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);

    while (*UART0_FR & (1 << 3)) {}
}

void uart_putc(char c)
{
    if (c == '\n')
        uart_putc('\r');

    while (*UART0_FR & (1 << 5))
        ;

    *UART0_DR = c;
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

void    uart_putnbr_u(unsigned int nb)
{
    if (nb == 0)
    {
        uart_putc('0');
        return;
    }
    uart_putnbr_llu((unsigned long long)nb);
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