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
#include <bcm2835.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
// #include <core.hpp>
// #include <imgcodecs.hpp>
// #include <highgui.hpp>
// #include "tjpgd3/decode_jpeg.h"
// #include "pngle/decode_png.h"
#include <utils.h>
#include <Type.h>
using namespace std; 
using namespace cv;

class display {
    public:
    static void init(int width, int height) 
    {
        if(isDebugMode())return;

        log("Initiating  ILI9340 LCD...", LogStatus::Loading);

        printf("Init\n");
        lcdInit(width, height, 0, 0);
        printf("Reset\n");
        lcdReset();
        printf("Setup\n");
        lcdSetup();

        lcdFillScreen(Colors::Black);
        
        log("Initiated screen!", LogStatus::Info);

            
        // FontxFile fx24G[2];
        // FontxFile fx16G[2];
        
        // Fontx_init(fx24G,"./lib/fontx/ILGH24XB.FNT","./lib/fontx/ILGZ24XB.FNT"); // 12x24Dot Gothic
        // Fontx_init(fx16G,"./lib/fontx/ILGH16XB.FNT","./lib/fontx/ILGZ16XB.FNT"); // 8x16Dot Gothic

        // FontxFile fx32M[2];
        // FontxFile fx24M[2];
        // FontxFile fx16M[2];
        // Fontx_init(fx32M,"./lib/fontx/ILMH32XF.FNT","./lib/fontx/ILMZ32XF.FNT"); // 16x32Dot Mincyo
        // Fontx_init(fx24M,"./lib/fontx/ILMH24XF.FNT","./lib/fontx/ILMZ24XF.FNT"); // 12x24Dot Mincyo
        // Fontx_init(fx16M,"./lib/fontx/ILMH16XB.FNT","./lib/fontx/ILMZ16XF.FNT"); // 8x16Dot Mincyo
    
        log("Loaded fonts successfully!", LogStatus::Success);
        
    }
    static void Clear() {
        lcdFillScreen(Colors::Black);
    }
    // Drawing functions
    static void drawCircle(int x1, int y1, int radius, ushort color, bool fill=false) {
        if(isDebugMode())return;
        
        if (fill) {
            lcdDrawFillCircle(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(radius), static_cast<uint16_t>(color));
            return;
        }
        lcdDrawCircle(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(radius), static_cast<uint16_t>(color));
    }
    // round rect
    static void drawRoundRect(int x1, int y1, int x2, int y2, ushort color, int radius, bool fill=false) 
    {
        if(isDebugMode())return;

        lcdDrawRoundRect(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(radius), static_cast<uint16_t>(color));
    }
    // rect
    static void drawRect(int x1, int y1, int x2, int y2, ushort color, bool fill=false)
    {
        if(isDebugMode())return;

        if(fill) {
            lcdDrawFillRect(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(color));
            return;
        }
        lcdDrawRect(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(color));
    }
    // line
    static void drawLine(int x1, int y1, int x2, int y2, ushort color) {
        if(isDebugMode())return;
        
        lcdDrawLine(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(color));
    }
    static void drawPixel(int x, int y, ushort color)
    {
        if(isDebugMode())return;

        lcdDrawPixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint16_t>(color));
    }

    // writing text to the display
    static void drawText(int x, int y, string text, ushort color)
    {
        if(isDebugMode())return;

        FontxFile fx32G[2];
        Fontx_init(fx32G,"./lib/fontx/ILGH32XB.FNT","./lib/fontx/ILGZ32XB.FNT"); // 16x32Dot Gothic

        // get font width & height
        uint8_t buffer[FontxGlyphBufSize];
        uint8_t fontWidth;
        uint8_t fontHeight;
        GetFontx(fx32G, 0, buffer, &fontWidth, &fontHeight);
        if(isDebugMode())printf("fontWidth=%d fontHeight=%d\n",fontWidth,fontHeight);

        uint8_t ascii[20];

        strcpy((char *)ascii, text.c_str());
        lcdSetFontDirection(DIRECTION0);
        lcdDrawUTF8String(fx32G, x, y, ascii, static_cast<uint16_t>(color));
    }


    // image draw
    static void drawImage(DataTypes::Point location, string file, int width, int height, bool reverse) 
    {
        if(isDebugMode())return;

        cv::Mat image = imread(file.c_str(), IMREAD_COLOR);
        

        if (image.empty()) {
            log("Could not open or find the image.", LogStatus::Error);
            return;
        }
        cv::Mat resizedImage;
        cv::Size newSize(width, height);
        cv::resize(image, resizedImage, newSize);

        // Iterate over each pixel in the image
        for (int y = 0; y < resizedImage.rows; y++) {
            for (int x = 0; x < resizedImage.cols; x++) {
                Vec3b pixel = resizedImage.at<Vec3b>(y, x); // Get the pixel value (RGB)

                // Access individual color channels
                int blue = pixel[0];
                int green = pixel[1];
                int red = pixel[2];

                drawPixel(location.X+x, location.Y+y, RGBtoRGB565(red, green, blue));

                // Example: Print the RGB values of each pixel
                // printf("Pixel at (%d, %d): R=%d, G=%d, B=%d\n", x, y, red, green, blue);
            }
        }
    }
};
#endif // !DISPLAY_H