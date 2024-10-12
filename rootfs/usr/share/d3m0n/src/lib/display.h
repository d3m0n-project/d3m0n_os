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
#include <fstream>
#include <functional>

#include <ili9340.h>
#include <bcm2835.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <cmath>
#include <bitset>
#include <stdint.h>
#include <unistd.h>
#include "xpt2046.h"



#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <utils.h>
#include <Type.h>
#include <memory>

#include "settings.h"
#include "utils.h"


using namespace std; 
using namespace cv;

#define T_CS BCM2835_SPI_CS1
#define RES RPI_V2_GPIO_P1_11
#define T_IRQ RPI_V2_GPIO_P1_22

namespace display {
    void setColorFromRGB(int r, int g, int b);

    extern GLFWwindow* window;
    extern vector<UIEvent::Listener> events;


    void registerEvent(short type, DataTypes::Rect rect, function<void(Application)> function, Application app);

    void getClick();

    bool checkCollision(DataTypes::Point point, DataTypes::Rect rect);
    void PlaySound(string name);
    string GetThemeIcon(string name);
    string displayCommand(string command);

    
    void init(int width, int height);
    void drawScreen();
    void Clear();


    // Drawing functions
    
    // pixel
    void drawPixel(int x, int y, ushort color);

    // circle
    void drawCircle(int x1, int y1, int radius, ushort color, bool fill=false);

    // rect
    void drawRect(int x1, int y1, int x2, int y2, ushort color, bool fill=false);

    // round rect
    void drawRoundRect(int x1, int y1, int x2, int y2, ushort color, int radius, bool fill=false);
    // line
    void drawLine(int x1, int y1, int x2, int y2, ushort color);

    // writing text to the display
    void drawText(int x, int y, int fontSize, string text, bool bold, ushort color);
    


    // image draw
    void drawImage(DataTypes::Point location, string file, int width, int height, bool reverse);
};



#endif