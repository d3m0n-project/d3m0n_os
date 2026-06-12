#ifndef SPI_H
#define SPI_H

#include "types.h"

void	spi_init(uint32_t baudrate);

void	spi_begin(void);
void	spi_end(void);

uint8_t	spi_transfer(uint8_t data);

void	spi_write_buffer(const void *buffer, uint32_t len);

#endif