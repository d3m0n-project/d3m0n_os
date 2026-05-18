#include "gpio.h"
#include "types.h"

#define GPIO_BASE 0x20200000

#define GPFSEL0 ((volatile uint32_t*)(GPIO_BASE + 0x00))
#define GPFSEL1 ((volatile uint32_t*)(GPIO_BASE + 0x04))
#define GPFSEL2 ((volatile uint32_t*)(GPIO_BASE + 0x08))

#define GPSET0  ((volatile uint32_t*)(GPIO_BASE + 0x1C))
#define GPSET1  ((volatile uint32_t*)(GPIO_BASE + 0x20))

#define GPCLR0  ((volatile uint32_t*)(GPIO_BASE + 0x28))
#define GPCLR1  ((volatile uint32_t*)(GPIO_BASE + 0x2C))

#define GPLEV0  ((volatile uint32_t*)(GPIO_BASE + 0x34))
#define GPLEV1  ((volatile uint32_t*)(GPIO_BASE + 0x38))


void	gpio_pinMode(int pin, int mode)
{
    volatile uint32_t *reg = (volatile uint32_t *)(GPIO_BASE + (pin / 10) * 4);
    int shift = (pin % 10) * 3;

    uint32_t val = *reg;
    val &= ~(7 << shift);        // clear function bits
    val |= (mode << shift);      // set mode
    *reg = val;
}


void	gpio_digitalWrite(int pin, int value)
{
    if (value)
    {
        if (pin < 32)
            *GPSET0 = (1 << pin);
        else
            *GPSET1 = (1 << (pin - 32));
    }
    else
    {
        if (pin < 32)
            *GPCLR0 = (1 << pin);
        else
            *GPCLR1 = (1 << (pin - 32));
    }
}

int		gpio_digitalRead(int pin)
{
    if (pin < 32)
        return ((*GPLEV0) >> pin) & 1;
    else
        return ((*GPLEV1) >> (pin - 32)) & 1;
}