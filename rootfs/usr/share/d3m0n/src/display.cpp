#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <sys/time.h>

#include "display.hpp"
// #include "ili9340.h"
//using namespace std; 

	
void display::init() 
{
	std::cout << "display init\n";
	// lcdReset();
	// lcdSetup();
	// lcdInit(display::width, display::height, 0, 0);
	
	// lcdFillScreen(backColor);
}
	// lcdWriteCommandByte(byte c);
    // lcdWriteDataByte(byte c);    
    // lcdWriteDataWord(ushort w);    
    // lcdWriteColor(ushort color, IntPtr size);    
    // lcdInit(int width, int height, int offsetx, int offsety);    
    // lcdReset();    
    // lcdSetup();    
    // lcdDrawPixel(ushort x, ushort y, ushort color);    
    // lcdDrawFillRect(ushort x1, ushort y1, ushort x2, ushort y2, ushort color);    
    // lcdDisplayOff();    
    // lcdDisplayOn();    
    // lcdInversionOn();    
    // lcdFillScreen(ushort color);    
    // lcdDrawLine(ushort x1, ushort y1, ushort x2, ushort y2, ushort color);    
    // lcdDrawRect(ushort x1, ushort y1, ushort x2, ushort y2, ushort color);    
    // lcdDrawCircle(ushort x0, ushort y0, ushort r, ushort color);    
    // lcdDrawFillCircle(ushort x0, ushort y0, ushort r, ushort color);    
    // lcdDrawRoundRect(ushort x1, ushort y1, ushort x2, ushort y2, ushort r, ushort color);    
    // lcdDrawArrow(ushort x0, ushort y0, ushort x1, ushort y1, ushort w, ushort color);    
    // lcdDrawFillArrow(ushort x0, ushort y0, ushort x1, ushort y1, ushort w, ushort color);    
    // lcdDrawSJISChar(IntPtr fx, ushort x, ushort y, ushort sjis, ushort color);    // [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
    // xxxxlcdDrawUTF8Char(FontxFile[] fx, ushort x, ushort y, string utf8, ushort color);    // [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
    // xxxxlcdDrawUTF8String(FontxFile[] fx, ushort x, ushort y, string utfs, ushort color);    
    // lcdSetFontDirection(ushort direction);    
    // lcdSetFontFill(ushort color);    
    // lcdUnsetFontFill();    
    // lcdSetFontUnderLine(ushort color);    
    // lcdUnsetFontUnderLine();