#include "spi.h"
#include "peripheral.h"
#include "gpio.h"

#define SPI0_BASE		(PERIPHERAL_BASE + 0x204000)

#define SPI0_CS			(*(volatile uint32_t *)(SPI0_BASE + 0x00))
#define SPI0_FIFO		(*(volatile uint32_t *)(SPI0_BASE + 0x04))
#define SPI0_CLK		(*(volatile uint32_t *)(SPI0_BASE + 0x08))

#define SPI_CS_RXF		(1 << 20)
#define SPI_CS_RXR		(1 << 19)
#define SPI_CS_TXD		(1 << 18)
#define SPI_CS_RXD		(1 << 17)
#define SPI_CS_DONE		(1 << 16)

#define SPI_CS_TA		 (1 << 7)

#define SPI_CS_CLEAR_RX	(1 << 5)
#define SPI_CS_CLEAR_TX	(1 << 4)

#define SPI_CS_CPOL		(1 << 3)
#define SPI_CS_CPHA		(1 << 2)

#define SPI_CORE_CLOCK	250000000

static uint8_t spi_active = 0;


#define SPI0_CS1_PIN	7
#define SPI0_CS0_PIN	8
#define SPI0_MISO_PIN	9
#define SPI0_MOSI_PIN	10
#define SPI0_SCLK_PIN	11


void	spi_init(uint32_t baudrate)
{
	uint32_t divider;

	gpio_set_alt(SPI0_CS1_PIN, GPIO_ALT0);
	gpio_set_alt(SPI0_CS0_PIN, GPIO_ALT0);
	gpio_set_alt(SPI0_MISO_PIN, GPIO_ALT0);
	gpio_set_alt(SPI0_MOSI_PIN, GPIO_ALT0);
	gpio_set_alt(SPI0_SCLK_PIN, GPIO_ALT0);


	if (baudrate == 0)
		baudrate = 1000000;

	divider = SPI_CORE_CLOCK / baudrate;

	if (divider < 2)
		divider = 2;

	if (divider > 65534)
		divider = 65534;

	divider &= ~1;

	SPI0_CS = SPI_CS_CLEAR_RX | SPI_CS_CLEAR_TX;


	SPI0_CS &= ~(SPI_CS_CPOL | SPI_CS_CPHA);

	SPI0_CLK = divider;

	spi_active = 0;
}


void	spi_begin(void)
{
	SPI0_CS = SPI_CS_CLEAR_RX | SPI_CS_CLEAR_TX;

	SPI0_CS |= SPI_CS_TA;

	spi_active = 1;
}


void	spi_end(void)
{
	while (!(SPI0_CS & SPI_CS_DONE))
		;

	while (SPI0_CS & SPI_CS_RXD)
		(void)SPI0_FIFO;

	SPI0_CS &= ~SPI_CS_TA;

	spi_active = 0;
}


uint8_t	spi_transfer(uint8_t data)
{
	//if (!spi_active) // TODO: re-enable
	//	spi_begin();


	while (!(SPI0_CS & SPI_CS_TXD))
		;


	SPI0_FIFO = data;


	while (!(SPI0_CS & SPI_CS_RXD))
		;


	return (uint8_t)SPI0_FIFO;
}


void	spi_write_buffer(const void *buffer, uint32_t len)
{
	const uint8_t *p = buffer;


	spi_begin();


	while (len--)
		spi_transfer(*p++);


	spi_end();
}