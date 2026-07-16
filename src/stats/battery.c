#include "stats.h"
#include "gpio.h"

#define PIN_CHARGING	22

int	is_charging(void)
{
	#if DEBUG == 1
	return 1;
	#else
	gpio_pinMode(PIN_CHARGING, GPIO_IN);
	return (gpio_digitalRead(PIN_CHARGING) > 0);
	#endif
}

int	get_battery_level(void)
{
	// TODO: battery level
	return 100;
}