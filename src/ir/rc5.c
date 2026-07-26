#include "ir.h"

#define HALF_BIT		889
#define CARRIER_FREQ	36000

static inline void rc5_mark(void)
{
	generate_freq(CARRIER_FREQ, IR_PIN, HALF_BIT);
}

static inline void rc5_space(void)
{
	gpio_digitalWrite(IR_PIN, 0);
	usleep(HALF_BIT);
}

void rc5_send_half(int carrier)
{
	if (carrier)
		rc5_mark();
	else
		rc5_space();
}

void ir_rc5_transmit(uint8_t address, uint8_t command)
{
	uint16_t frame  =	(1 << 13) |	   // start bit 1
						(1 << 12) |	   // start bit 2
						(1 << 11) |	   // toggle bit
						((address & 0x1F) << 6) |
						(command & 0x3F);

	for (int i = 13; i >= 0; i--)
	{
		uint8_t bit = (frame >> i) & 1;
		if (bit)
		{
			rc5_space();
			rc5_mark();
		}
		else
		{
			rc5_mark();
			rc5_space();
		}
	}
	gpio_digitalWrite(IR_PIN, 0);
}