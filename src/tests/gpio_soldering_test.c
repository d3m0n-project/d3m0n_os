#include "tests/tests.h"
#include "peripheral/gpio.h"

#define COLOR_I2C		ANSI_COLOR_MAGENTA
#define COLOR_POWER		ANSI_COLOR_RED
#define COLOR_GROUND	ANSI_COLOR_GREY
#define COLOR_RF		ANSI_COLOR_CYAN
#define COLOR_SPI		ANSI_COLOR_YELLOW
#define COLOR_MISC		ANSI_COLOR_GREEN
#define COLOR_UART		ANSI_COLOR_BLUE
#define COLOR_UNKNOWN	ANSI_COLOR_RESET

typedef struct
{
	int				gpio_id;
	const char		*name;
	const char		*ansi_color;
}	t_pin_schematic;

#define NOT_A_GPIO		-1

static const t_pin_schematic	pin_schematic[] = {
	{NOT_A_GPIO,		"3V3",				COLOR_POWER},
	{NOT_A_GPIO,		"5V",				COLOR_POWER},
	{2,					"I2C SDA",			COLOR_I2C},
	{NOT_A_GPIO,		"5V",				COLOR_POWER},
	{3,					"I2C SCL",			COLOR_I2C},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{4,					"RF ANT1",			COLOR_RF},
	{14,				"LTE TX",			COLOR_UART},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{15,				"LTE RX",			COLOR_UART},
	{17,				"LORA CS",			COLOR_SPI},
	{18,				"GPIO UART TX",		COLOR_UART},
	{27,				"SCREEN RESET",		COLOR_SPI},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{22,				"IS CHARGING",		COLOR_MISC},
	{23,				"GDO0",				COLOR_MISC},
	{NOT_A_GPIO,		"3V3",				COLOR_POWER},
	{24,				"GDO2",				COLOR_MISC},
	{10,				"SCREEN MOSI",		COLOR_SPI},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{9,					"SCREEN MISO",		COLOR_SPI},
	{25,				"SCREEN DC",		COLOR_SPI},
	{11,				"SCREEN SCL",		COLOR_SPI},
	{8,					"SCREEN CS",		COLOR_SPI},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{7,					"GPIO UART RX",		COLOR_UART},
	{NOT_A_GPIO,		"ID_SD",			COLOR_UNKNOWN},
	{NOT_A_GPIO,		"ID_SC",			COLOR_UNKNOWN},
	{5,					"SCREEN LED",		COLOR_MISC},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{6,					"POWER BTN",		COLOR_MISC},
	{12,				"IR SEND",			COLOR_MISC},
	{13,				"IR RECV",			COLOR_MISC},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{19,				"SPI MISO2",		COLOR_SPI},
	{16,				"SPI2 CS",			COLOR_SPI},
	{26,				"LTE PWR",			COLOR_MISC},
	{16,				"SPI MOSI2",		COLOR_SPI},
	{NOT_A_GPIO,		"GND",				COLOR_GROUND},
	{21,				"SPI SCL2",			COLOR_SPI}
};

bool	test_pin(t_pin_schematic pin)
{
	// not a gpio, skip
	if (pin.gpio_id < 0)
		return true;
	
	int result = gpio_digitalRead(pin.gpio_id);
	return (result == 1);
	//int _tmp = uart_getc();
}

void	gpio_soldering_test(void)
{
	log("Starting GPIO Soldering Tests...\n\n", LOG_INFO);
	while (1)
	{
		log("   %-15s   %s+----------+%s\n", LOG_NONE, "", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);

		size_t total_pins = sizeof(pin_schematic) / sizeof(pin_schematic[0]);
		for (size_t i=0; i < total_pins; i+=2)
		{
			t_pin_schematic pin1 = pin_schematic[i];
			if (pin1.gpio_id >= 0)
				gpio_pinMode(pin1.gpio_id, GPIO_IN);
			
			t_pin_schematic pin2 = {NOT_A_GPIO, "NA", COLOR_UNKNOWN};
			if (i + 1 < total_pins)
			{
				pin2 = pin_schematic[i + 1];
				if (pin2.gpio_id >= 0)
					gpio_pinMode(pin2.gpio_id, GPIO_IN);
			}

			char checked1 = test_pin(pin1)?'x':' ';
			char checked2 = test_pin(pin2)?'x':' ';
			const char *bg_cell_color = "\033[48;2;255;225;160m";
			log("   %s%15s%s   ", LOG_NONE, pin1.ansi_color, pin1.name, ANSI_COLOR_YELLOW);
			log("%s| %s[%c]  [%c]%s %s|", LOG_NONE,
				ANSI_COLOR_YELLOW, bg_cell_color, checked1, checked2, ANSI_COLOR_RESET, ANSI_COLOR_YELLOW
			);
			log("   %s%-15s%s   \n", LOG_NONE, pin2.ansi_color, pin2.name, ANSI_COLOR_RESET);
		}
		log("   %-15s   %s+----------+%s\n", LOG_NONE, "", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
		log("Press any key to restart test or press CTRL+C to exit\n", LOG_INFO);
		int pressed_key = uart_getc();
		if (pressed_key == 3) // CTRL+C
			break;
		uart_clear();
	}
}