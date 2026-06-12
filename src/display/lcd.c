//#include "lcd.h"
#include "gpio.h"
#include "spi.h"
#include "display.h"
#include "time.h"

#define PIN_DC		25
#define PIN_RST		24
#define PIN_BL		18

static void	lcd_cmd(uint8_t c)
{
	gpio_digitalWrite(PIN_DC, 0);
	spi_write(c);
}

static void	lcd_data(uint8_t d)
{
	gpio_digitalWrite(PIN_DC, 1);
	spi_write(d);
}

static void	lcd_reset()
{
	gpio_digitalWrite(PIN_RST, 0);
	usleep(100000);
	gpio_digitalWrite(PIN_RST, 1);
	usleep(50000);
}

static void	lcd_write_color(uint16_t c)
{
	lcd_data(c >> 8);
	lcd_data(c & 0xFF);
}

static void	lcd_set_window(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{
	lcd_cmd(0x2A);
	lcd_data(x0 >> 8); lcd_data(x0);
	lcd_data(x1 >> 8); lcd_data(x1);

	lcd_cmd(0x2B);
	lcd_data(y0 >> 8); lcd_data(y0);
	lcd_data(y1 >> 8); lcd_data(y1);

	lcd_cmd(0x2C);
}

void	lcd_clear(uint16_t color)
{
	lcd_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

	gpio_digitalWrite(PIN_DC, 1);

	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		lcd_write_color(color);
	}
}

int	lcd_init()
{
	gpio_pinMode(PIN_DC, GPIO_OUT);
	gpio_pinMode(PIN_RST, GPIO_OUT);
	gpio_pinMode(PIN_BL, GPIO_OUT);

	spi_init(0); // TODO: change speed

	lcd_reset();

	lcd_cmd(0xF7);
	lcd_data(0xA9); lcd_data(0x51); lcd_data(0x2C); lcd_data(0x82);

	lcd_cmd(0xC0); lcd_data(0x11); lcd_data(0x09);
	lcd_cmd(0xC1); lcd_data(0x41);
	lcd_cmd(0xC5); lcd_data(0x00); lcd_data(0x0A); lcd_data(0x80);

	lcd_cmd(0x36);
	lcd_data(0x48);

	lcd_cmd(0x3A);
	lcd_data(0x66);

	lcd_cmd(0x11);
	usleep(120000);

	lcd_cmd(0x29);

	gpio_digitalWrite(PIN_BL, 1);
	return 0;
}