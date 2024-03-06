#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <sys/time.h>
#include <algorithm>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

#include <ili9340.h>
// #include <bcm2835.h>
#include <utils.h>
using namespace std; 

class display {
    public:
    static void init(int width, int height) 
    {
        log("Initiating  ILI9340 LCD...", LogStatus::Loading);
        if(!isDebugMode()) {
            lcdReset();
            lcdSetup();
            lcdFillScreen(Colors::Black);
            lcdInit(width, height, 0, 0);
            log("Initiated screen!", LogStatus::Info);
        }
    }
};
#endif // !DISPLAY_H