#ifndef GPIO_H
#define GPIO_H

#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

#define GPIO_IN			0
#define GPIO_OUT		1

#define GPIO_ALT0		4

void	gpio_pinMode(int pin, int mode);
void	gpio_digitalWrite(int pin, int value);
int		gpio_digitalRead(int pin);
void	gpio_set_alt(int pin, int alt);

#endif