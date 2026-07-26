#ifndef FREQUENCY_GENERATOR_H
#define FREQUENCY_GENERATOR_H

#include "types.h"
#include "gpio.h"
#include "time.h"

void	generate_freq(float freq, int pin, uint64_t t);

#endif