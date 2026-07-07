#ifndef LCD_H
#define LCD_H

#include "types.h"

#define PIN_LCD_DC		25
#define PIN_LCD_RST		27

void	lcd_clear(uint16_t color);
int		lcd_init();
void	lcd_drawrect(int x, int y, int w, int h, uint16_t color);

// TODO: remove me
void	lcd_cmd(uint8_t c);


#endif