#include "frequency_generator.h"


// generates a frequency in Hz for t us
void	generate_freq(float freq, int pin, uint64_t t)
{
	if (freq <= 0)
		return;

	gpio_pinMode(pin, GPIO_OUT);

	uint32_t half_period = (uint32_t)(500000.0f / freq); // us
	uint64_t start_t = time_us();
	int d = 0;
	while (time_us() - start_t < t)
	{
		gpio_digitalWrite(pin, (d & 1));
		usleep(half_period);
		d ^= d;
	}
	gpio_digitalWrite(pin, 0);
}