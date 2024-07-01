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
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <cmath>


#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
// #include "tjpgd3/decode_jpeg.h"
// #include "pngle/decode_png.h"
#include <utils.h>
#include <Type.h>


using namespace std; 
using namespace cv;

class display {
    private:
    void setColorFromRGB(int r, int g, int b) {
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
    }

    public:
    static void PlaySound(string name)
	{
		string theme_path = GetPath()+"/themes/"+getSetting("theme", GetConfig())+"/sounds";
		try
		{
			if(filesystem::exists(theme_path+"/"+name+".ogg"))
			{
                exec(("paplay '"+theme_path+"/"+name+".ogg'").c_str());
				log("Playing '"+theme_path+"/"+name+".ogg'...", LogStatus::Info);
			}
			else
			{
                log("sound '"+name+"' is not found", LogStatus::Error);
			}
		}
		catch(exception)
		{
			log("Could not find command 'paplay' >> can't play sound file", LogStatus::Error);
		}
	}
    static string GetThemeIcon(string name) {
        string theme_path = GetPath()+"/themes/"+getSetting("theme", GetConfig())+"/icons";
        if(filesystem::exists(theme_path+"/"+name+".png"))
        {
            return theme_path+"/"+name+".png";
        }
        else
        {
            return GetPath()+"/default.png";
        }
    }
    string displayCommand(string command) {
        string output="";

        // if(isDebugMode()) { return "Debug Mode, Command not executed."; }

        // clear screen
        if(startsWith(command, "display clear")) { 
            output+=log("Cleared display screen successfully!", LogStatus::Success); 
            Clear(); 
        }
        // draw Controls on screen
        // else if(startsWith(command, "display draw ")) { output+=log("Cleared display screen successfully!", LogStatus::Success); lcdFillScreen(Colors::Black); };
        else {
            output+=log("Usage: 'display [clear/draw] <color> ...'", LogStatus::Error);
        }

        return output;
    }
    
    GLFWwindow *window;
    
    void init(int width, int height) 
    {
        
        if(isDebugMode()) {
            if (!glfwInit())
            {
                log("Error Could not start opengl!", LogStatus::Critical);
                return;
            }

            // create a window pointer
            window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
            // error check
            if (window == NULL)
            {
                log("Error could not create emulator's window!", LogStatus::Critical);
                glfwTerminate();
                return;
            }

            glfwMakeContextCurrent(window);

            glewExperimental = GL_TRUE;
            glewInit();

            // glClear(GL_COLOR_BUFFER_BIT);
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            gluOrtho2D(0.0, width, height, 0.0);

            // drawPixel(100, 100, Colors::Red);
            // drawScreen();
            // drawPixel(100, 150, Colors::Yellow);
            
            
            // glfwSwapBuffers(window);
            // glfwPollEvents();
        } else {

            log("Initiating  ILI9340 LCD...", LogStatus::Loading);

            printf("Init\n");
            lcdInit(width, height, 0, 0);
            printf("Reset\n");
            lcdReset();
            printf("Setup\n");
            lcdSetup();

            lcdFillScreen(Colors::Black);
            
            

                
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
        log("Initiated screen!", LogStatus::Info);
    }
    void drawScreen() {
        glfwSwapBuffers(window);
    }
    void Clear() {
        if (isDebugMode()) {
            glClear(GL_COLOR_BUFFER_BIT);
        } else {
            lcdFillScreen(Colors::Black);
        }
    }
    // Drawing functions
    void drawCircle(int x1, int y1, int radius, ushort color, bool fill=false) {
        
        if(isDebugMode()) {
            float t1 = radius / 16;
            float t2;
            int x = radius;
            int y = 0;
            while(x < y) {
                drawPixel(x, y, color);
                // Pixel (x, y) and all symmetric pixels are colored (8 times)
                y = y + 1;
                t1 = t1 + y;
                t2 = t1 - x;
                if(t2 >= 0) {
                    t1 = t2;
                    x = x - 1;
                }
                    
            }

            
        } else {
            if (fill) {
                lcdDrawFillCircle(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(radius), static_cast<uint16_t>(color));
                return;
            }
            lcdDrawCircle(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(radius), static_cast<uint16_t>(color));
        }
    }

    // round rect
    void drawRoundRect(int x1, int y1, int x2, int y2, ushort color, int radius, bool fill=false) 
    {
        
        log("RoundRect not def", LogStatus::Critical);

        if(isDebugMode())return;

        lcdDrawRoundRect(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(radius), static_cast<uint16_t>(color));
    }
    // rect
    void drawRect(int x1, int y1, int x2, int y2, ushort color, bool fill=false)
    {
        
        if(isDebugMode()) {
            // uint8_t r;
            // uint8_t g;
            // uint8_t b;
            // int isfilled=0;
            // RGB565toRGB((uint16_t)color, r, g, b);

            if(fill) {

            } else {
                drawLine(x1, y1, x2, y1, color);
                drawLine(x1, y1, x1, y2, color);
                drawLine(x1, y2, x2, y2, color);
                drawLine(x2, y1, x2, y2, color);
            }
        } else {
            if(fill) {
                lcdDrawFillRect(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(color));
                return;
            }
            lcdDrawRect(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(color));
        }
    }
    // line
    void drawLine(int x1, int y1, int x2, int y2, ushort color) {
        
        if(isDebugMode()) {
            glBegin(GL_LINES);
            uint8_t r,g,b;
            RGB565toRGB(color, r, g, b);
            setColorFromRGB(r,g,b);

            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glEnd();
            drawScreen();
        } else {
            lcdDrawLine(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(x2), static_cast<uint16_t>(y2), static_cast<uint16_t>(color));
        }
    }
    void drawPixel(int x, int y, ushort color)
    {
        
        if(isDebugMode()) {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            RGB565toRGB((uint16_t)color, r, g, b);
            // logn(to_string(r)+","+to_string(g)+","+to_string(b)+" ==> "+to_string(color), ConsoleColor::Red);
            // Vec3b color2((int)r, (int)g, (int)b); // Red color (BGR format)
            // // Draw the pixel
            // image.at<Vec3b>(y, x) = color2;
            // // Display the image in the window
            // imshow(window_name, image);
            glBegin(GL_POINTS);
            setColorFromRGB(r, g, b);
            glVertex2i(x,y);
            glEnd();
        } else {
            lcdDrawPixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint16_t>(color));
        }
    }
    // writing text to the display
    void drawText(int x, int y, int fontSize, string text, bool bold, ushort color)
    {
        
        // return;
        if(isDebugMode()) {
            // uint8_t r;
            // uint8_t g;
            // uint8_t b;
            // RGB565toRGB((uint16_t)color, r, g, b);

            // putText(image, text, Point(x, y), cv::FONT_HERSHEY_COMPLEX, 0.5, Scalar((int)r, (int)g, (int)b), 1);
            // // putText(image, text, Point(x, y), FONT_HERSHEY_SIMPLEX, 1.0, Scalar((int)r, (int)g, (int)b), 2);
            // imshow(window_name, image);
        } else {
            string font1 = GetPath()+"/src/lib/fontx/IL";
            string end_font1 = "";
            string fontName = "MINCYO";

            if(fontName == "GOTHIC") { font1+="G"; } else {
                font1+="M";
            }
            if(bold) {
                end_font1="XF.FNT";
            }
            else{
                end_font1="XB.FNT";
            }


            FontxFile fx32G[2];
            printf((font1+"H"+to_string(fontSize)+end_font1).c_str());
            Fontx_init(fx32G,(font1+"H"+to_string(fontSize)+end_font1).data(),(font1+"Z"+to_string(fontSize)+end_font1).data());

            // get font width & height
            uint8_t buffer[FontxGlyphBufSize];
            uint8_t fontWidth;
            uint8_t fontHeight;
            GetFontx(fx32G, 0, buffer, &fontWidth, &fontWidth);

            uint8_t ascii[20];

            strcpy((char *)ascii, text.c_str());
            lcdSetFontDirection(DIRECTION180);
            lcdDrawUTF8String(fx32G, x, y, ascii, static_cast<uint16_t>(color));
        }
    }


    // image draw
    void drawImage(DataTypes::Point location, string file, int width, int height, bool reverse) {
        logn("img X:" + to_string(location.X) + " Y:" + to_string(location.Y), ConsoleColor::DarkCyan);
        logn("width:" + to_string(width) + " height:" + to_string(height), ConsoleColor::DarkCyan);

        // Load the image (IMREAD_UNCHANGED to keep alpha if any)
        Mat image2 = imread(file.c_str(), IMREAD_UNCHANGED);

        if (image2.empty()) {
            log("Could not open or find the image.", LogStatus::Error);
            return;
        }

        // Resize the image
        Mat resizedImage;
        Size newSize(width, height);
        resize(image2, resizedImage, newSize);

        return;
        // Determine if the loaded image has an alpha channel
        bool hasAlpha = (resizedImage.channels() == 4);

        int r, g, b;
        ushort color;
        for (int y = 0; y < resizedImage.rows; y++) {
            for (int x = 0; x < resizedImage.cols; x++) {
                int alpha=0;
                
                Vec3b pixel = resizedImage.at<Vec3b>(y, x); // Get the pixel value (RGB)
                if(hasAlpha) {
                    alpha = pixel[3]; // Get alpha channel
                }


                if (isDebugMode()) {
                    if (alpha > 0) {
                        // color = RGBtoRGB565(r, g, b);
                        color = Colors::Cyan;
                        drawPixel(x+location.X, y+location.Y, color);
                    }
                } else {
                    if (alpha > 0) {
                        lcdDrawPixel(static_cast<uint16_t>(SCREEN_WIDTH - (location.X + x)), static_cast<uint16_t>(location.Y + y), static_cast<uint16_t>(RGBtoRGB565(pixel[2], pixel[1], pixel[0])));
                    }
                }
                // cv::waitKey(0);
            }
        }
        drawScreen();
    }
};
#endif // !DISPLAY_H