#include "lcd.h"
#include "gpio.h"
#include "spi.h"
#include "display.h"
#include "time.h"

#define PIN_DC		25
#define PIN_RST		27
#define PIN_BL		18 // HARDWARE: TODO: add pin 18 for backlight

static void	lcd_cmd(uint8_t c)
{
	gpio_digitalWrite(PIN_DC, 0);
	spi_transfer(c);
}

static void	lcd_data(uint8_t d)
{
	gpio_digitalWrite(PIN_DC, 1);
	spi_transfer(d);
}

static void	lcd_reset(void)
{
	gpio_digitalWrite(PIN_RST, 0);
	usleep(100000);
	gpio_digitalWrite(PIN_RST, 1);
	usleep(50000);
}

static inline void	lcd_write_color(uint16_t color)
{
	/* RGB565 -> RGB666 */
	spi_transfer((color >> 8) & 0xF8);
	spi_transfer((color >> 3) & 0xFC);
	spi_transfer(color << 3);
}

static void	lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	lcd_cmd(0x2A);
	lcd_data(x0 >> 8);
	lcd_data(x0);
	lcd_data(x1 >> 8);
	lcd_data(x1);

	lcd_cmd(0x2B);
	lcd_data(y0 >> 8);
	lcd_data(y0);
	lcd_data(y1 >> 8);
	lcd_data(y1);

	lcd_cmd(0x2C);
}

void	lcd_clear(uint16_t color)
{
	lcd_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

	gpio_digitalWrite(PIN_DC, 1);

	for (uint32_t i = 0; i < (uint32_t)SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		lcd_write_color(color);
}

int	lcd_init(void)
{
	gpio_pinMode(PIN_DC, GPIO_OUT);
	gpio_pinMode(PIN_RST, GPIO_OUT);
	gpio_pinMode(PIN_BL, GPIO_OUT);

	spi_init(50000000); // 50 MHz

	lcd_reset();

	lcd_cmd(0xF7);
	lcd_data(0xA9);
	lcd_data(0x51);
	lcd_data(0x2C);
	lcd_data(0x82);

	lcd_cmd(0xC0);
	lcd_data(0x11);
	lcd_data(0x09);

	lcd_cmd(0xC1);
	lcd_data(0x41);

	lcd_cmd(0xC5);
	lcd_data(0x00);
	lcd_data(0x0A);
	lcd_data(0x80);

	lcd_cmd(0xB1);
	lcd_data(0xB0);
	lcd_data(0x11);

	lcd_cmd(0xB4);
	lcd_data(0x02);

	lcd_cmd(0xB6);
	lcd_data(0x02);
	lcd_data(0x42);

	lcd_cmd(0xB7);
	lcd_data(0xC6);

	lcd_cmd(0xBE);
	lcd_data(0x00);
	lcd_data(0x04);

	lcd_cmd(0xE9);
	lcd_data(0x00);

	lcd_cmd(0x36);
	lcd_data((1 << 3) | (0 << 7) | (1 << 6) | (1 << 5));

	lcd_cmd(0x3A);
	lcd_data(0x66);

	lcd_cmd(0xE0);
	lcd_data(0x00);
	lcd_data(0x07);
	lcd_data(0x10);
	lcd_data(0x09);
	lcd_data(0x17);
	lcd_data(0x0B);
	lcd_data(0x41);
	lcd_data(0x89);
	lcd_data(0x4B);
	lcd_data(0x0A);
	lcd_data(0x0C);
	lcd_data(0x0E);
	lcd_data(0x18);
	lcd_data(0x1B);
	lcd_data(0x0F);

	lcd_cmd(0xE1);
	lcd_data(0x00);
	lcd_data(0x17);
	lcd_data(0x1A);
	lcd_data(0x04);
	lcd_data(0x0E);
	lcd_data(0x06);
	lcd_data(0x2F);
	lcd_data(0x45);
	lcd_data(0x43);
	lcd_data(0x02);
	lcd_data(0x0A);
	lcd_data(0x09);
	lcd_data(0x32);
	lcd_data(0x36);
	lcd_data(0x0F);

	lcd_cmd(0x11);
	usleep(120000);

	lcd_cmd(0x29);

	gpio_digitalWrite(PIN_BL, 1);

	return 0;
}