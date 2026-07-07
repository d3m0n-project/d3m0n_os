#include "lcd.h"
#include "gpio.h"
#include "spi.h"
#include "display.h"
#include "time.h"

static uint8_t		setxcmd = 0x2A;
static uint8_t		setycmd = 0x2B;
static uint8_t		wramcmd = 0x2C;

static uint16_t		lcd_width  = SCREEN_WIDTH;
static uint16_t		lcd_height = SCREEN_HEIGHT;


void	lcd_cmd(uint8_t c)
{
	gpio_digitalWrite(PIN_LCD_DC, 0);
	spi_transfer(c);
}


static void	lcd_data(uint8_t d)
{
	gpio_digitalWrite(PIN_LCD_DC, 1);
	spi_transfer(d);
}


static void	lcd_reset(void)
{
	gpio_digitalWrite(PIN_LCD_RST, 0);
	usleep(100000);

	gpio_digitalWrite(PIN_LCD_RST, 1);
	usleep(50000);
}


static void	lcd_direction(uint8_t direction)
{
	if (direction == 0)
	{
		lcd_width = SCREEN_WIDTH;
		lcd_height = SCREEN_HEIGHT;

		lcd_cmd(0x36);
		lcd_data((1 << 3) | (0 << 6) | (0 << 7));
	}
	else if (direction == 1)
	{
		lcd_width = SCREEN_HEIGHT;
		lcd_height = SCREEN_WIDTH;

		lcd_cmd(0x36);
		lcd_data((1 << 3) | (1 << 6) | (1 << 5));
	}
}


static inline void	lcd_write_color(uint16_t color)
{
	spi_transfer((color >> 8) & 0xF8);
	spi_transfer((color >> 3) & 0xFC);
	spi_transfer(color << 3);
}


static void	lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	lcd_cmd(setxcmd);

	lcd_data(x0 >> 8);
	lcd_data(x0);
	lcd_data(x1 >> 8);
	lcd_data(x1);


	lcd_cmd(setycmd);

	lcd_data(y0 >> 8);
	lcd_data(y0);
	lcd_data(y1 >> 8);
	lcd_data(y1);


	lcd_cmd(wramcmd);
}

void	lcd_drawrect(int x, int y, int w, int h, uint16_t color)
{
	spi_begin();
    lcd_set_window(x, y, x+w, y+h);
    gpio_digitalWrite(PIN_LCD_DC, 1);
	for (int i=0; i<w*h; i++)
    	lcd_write_color(color);
    spi_end();
}


void	lcd_clear(uint16_t color)
{
	spi_begin();

	lcd_set_window(0, 0, lcd_width - 1, lcd_height - 1);

	gpio_digitalWrite(PIN_LCD_DC, 1);

	for(uint32_t i=0; i< (uint32_t)lcd_width * lcd_height; i++)
		lcd_write_color(color);

	spi_end();
}


int	lcd_init(void)
{
	gpio_pinMode(PIN_LCD_DC, GPIO_OUT);
	gpio_pinMode(PIN_LCD_RST, GPIO_OUT);


	spi_init(50000000); // 50MHz
	lcd_reset();

	spi_begin();


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

	spi_end();

	usleep(120000);


	spi_begin();
	lcd_cmd(0x29);
	spi_end();

	usleep(10000);

	spi_begin();
	lcd_direction(0);
	spi_end();

	return 0;
}