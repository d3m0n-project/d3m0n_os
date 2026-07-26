#include "ir.h"
#include "log.h"

#define BURST_LENGTH	562
#define CARRIER_FREQ	38222

inline void	ir_nec_transmit_byte(uint8_t byte)
{
	for (int i=0; i<8; i++)
	{
		generate_freq(CARRIER_FREQ, IR_PIN, BURST_LENGTH);
		usleep(BURST_LENGTH);
		if (byte & (1 << i))
			usleep(BURST_LENGTH);
	}
}

void	ir_nec_transmit(uint8_t address, uint8_t command)
{
	generate_freq(CARRIER_FREQ, IR_PIN, 9000);
	usleep(4500);
	ir_nec_transmit_byte(address);
	ir_nec_transmit_byte(~address);
	ir_nec_transmit_byte(command);
	ir_nec_transmit_byte(~command);

	// final burst
	generate_freq(CARRIER_FREQ, IR_PIN, BURST_LENGTH);
	gpio_digitalWrite(IR_PIN, 0);
}