#ifndef GPIO_H
#define GPIO_H

#define GPIO_IN  0
#define GPIO_OUT 1

void	gpio_pinMode(int pin, int mode);
void	gpio_digitalWrite(int pin, int value);
int		gpio_digitalRead(int pin);

#endif