#include "ir.h"

void	ir_transmit(uint8_t address, uint8_t command, e_ir_protocol protocol)
{
	gpio_pinMode(IR_PIN, GPIO_OUT);
	switch (protocol)
	{
		case IR_NEC:
			ir_nec_transmit(address, command);
			break;
		
		case IR_PHILIPS_RC5:
			ir_rc5_transmit(address, command);
			break;
		
		default:
			break;
	}
}