#ifndef IR_H
#define IR_H

#include "types.h"
#include "gpio.h"
#include "time.h"
#include "frequency_generator.h"

#define IR_PIN			5

typedef enum
{
	IR_NEC,
	IR_PHILIPS_RC5
}	e_ir_protocol;


void	ir_transmit(uint8_t address, uint8_t command, e_ir_protocol protocol);

// nec
void	ir_nec_transmit(uint8_t address, uint8_t command);

// philips-rc5
void	ir_rc5_transmit(uint8_t address, uint8_t command);

#endif