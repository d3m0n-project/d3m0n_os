#ifndef SPI_H
#define SPI_H

#include "types.h"

void	spi_init(uint32_t baudrate);
void	spi_write(uint8_t data);

#endif