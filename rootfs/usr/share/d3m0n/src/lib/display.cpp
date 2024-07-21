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


using namespace std; 
using namespace cv;

#define T_CS BCM2835_SPI_CS1
#define RES RPI_V2_GPIO_P1_11
#define T_IRQ RPI_V2_GPIO_P1_22

class display {
    private:
    void setColorFromRGB(int r, int g, int b) {
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
    }
    
    public:
    GLFWwindow* window;
    vector<Event::Listener> events;

    void registerEvent(short type, DataTypes::Rect rect, function<void(Application*)> function, Application* app) {
        log("Registering new event id:"+to_string(type), LogStatus::Loading);
        
        Event::Listener listener;
        listener.type = type;
        listener.rect = rect;
        listener.callback = function;
        listener.app = app;

        events.push_back(listener);
    }

    void getClick() {
        if(isDebugMode()) {
            double xpos, ypos;
            int state;
            bool clicking=false;
            while(true) {
                state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

                if (state == GLFW_PRESS && !clicking)
                {
                    clicking=true;
                    glfwGetCursorPos(window, &xpos, &ypos);

                    for(Event::Listener event: events) {
                        if(event.type == Event::OnClick) {
                            if(checkCollision(DataTypes::Point((int)xpos, (int)ypos), event.rect)) {
                                event.callback(event.app);
                            }
                        }
                    }
                    //usleep(10000);
                } else if(state == GLFW_RELEASE) {
                    clicking=false;
                }
                // cout << to_string(clicking);

                glfwPollEvents();
            }
            
            
        } else {
            int	x, y;
            int	pen_irq;

            bcm2835_gpio_write(RES, LOW);
            bcm2835_delay(100);
            bcm2835_gpio_write(RES, HIGH);
            bcm2835_delay(100);

            while(true) {
                usleep(10000);	/* do it anyway ; settle time when pen goes up */
                pen_irq = bcm2835_gpio_lev(T_IRQ);
                if (pen_irq == LOW)	{ /* P1.22 == PenIRQ is LOW : touch! pen is down */
                    xptGetxy(T_CS, &x, &y);
                    printf("touch !! x=%5d y=%5d\n", x, y);
                } 
            }
            

            bcm2835_close();
        }
    }

    static bool checkCollision(DataTypes::Point point, DataTypes::Rect rect) {
        if(point.X > rect.X && point.X < rect.X2) {
            if(point.Y > rect.Y && point.Y < rect.Y2) {
                return true;
            }
        }
        return false;
    }
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

    
    void init(int width, int height) 
    {
        if(isDebugMode()) {
            if (!glfwInit()) {
                log("Error: Could not start OpenGL!", LogStatus::Critical);
                return;
            }

            // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // Create a windowed mode window and its OpenGL context
            window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
            if (!window) {
                log("Error: Could not create emulator's window!", LogStatus::Critical);
                glfwTerminate();
                return;
            }

            
            glfwMakeContextCurrent(window);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


            // Initialize GLEW
            glewExperimental = GL_TRUE; // Needed for core profile
            if (glewInit() != GLEW_OK) {
                log("Error: Failed to initialize GLEW", LogStatus::Critical);
                glfwTerminate();
                return;
            }

            

            // Set up orthographic projection
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0.0, width, height, 0.0);
            glMatrixMode(GL_MODELVIEW);

            // return window;
        } else {
            log("Initiating  ILI9340 LCD...", LogStatus::Loading);
            
            if (bcm2835_init() != 1) return;
            bcm2835_gpio_fsel(T_IRQ, BCM2835_GPIO_FSEL_INPT); // T_IRQ
            bcm2835_gpio_fsel(RES, BCM2835_GPIO_FSEL_OUTP); // RES


            

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
        if(isDebugMode()) {
            glfwSwapBuffers(window);
        }
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
                int height=y2-y1;
                int width=x2-x1;
                for(int y=0; y<height; y++) {
                    for(int x=0; x<width; x++) {
                        drawPixel(x1+x, y1+y, color);
                    }
                }
                drawScreen();
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
            string filePath = "./lib/fontx/ILGH16XB.FNT";
            
            vector<unsigned char> data;

            if (readAllBytes(filePath, data)) {
                cout << "Successfully read " << data.size() << " bytes from the font file." << endl;
                // ofstream myfile;
                // myfile.open("output.txt");

                int line=0;
                int font_padding = 2;
                for(char c: text) {
                    int offset = 17+static_cast<int>(c)*16;

                    for(int x2=offset; x2<offset+16; x2++) {
                        bitset<8> bits(data[x2]);
                        int bitOffsetInByte = 0;
                        for(char a: bits.to_string()) {
                            if(a=='1') {
                                glBegin(GL_POINTS);
                                setColorFromRGB(255, 255, 255);
                                glVertex2i(x+bitOffsetInByte,y+line);
                                glEnd();
                            }
                            bitOffsetInByte++;
                        }
                        // cout << "\n";
                        line++;
                    }

                    x+=8;
                    line=0;

                    
                }
                drawScreen();
                
                return;
                
                int y=0;
                for(int x=17+48*16*8; x<17+48*16*8+16*8; x++) {
                    bitset<8> bits(data[x]);
                    if(y%8==0) {
                        cout << bits.to_string();
                        // for(char a: bits.to_string()) {
                        //     if(a=='1') {
                        //         cout << "#"; 
                        //     } else {
                        //         cout << " ";
                        //     }
                        // }
                        cout << endl;
                    } else {
                        cout << bits.to_string();
                        // for(char a: bits.to_string()) {
                        //     if(a=='1') {
                        //         cout << "#"; 
                        //     } else {
                        //         cout << " ";
                        //     }
                        // }
                    }
                    y+=8;
                }
                
            } else {
                cerr << "Error reading the file." << endl;
            }
            
            
            //get_font("./lib/fontx/ILGH16XB.FNT", static_cast<int>('0'));
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
        // logn("img X:" + to_string(location.X) + " Y:" + to_string(location.Y), ConsoleColor::DarkCyan);
        // logn("width:" + to_string(width) + " height:" + to_string(height), ConsoleColor::DarkCyan);

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
        
        // Determine if the loaded image has an alpha channel
        bool hasAlpha = (resizedImage.channels() == 4);

        int x = 0, y = 0;
        float alpha = 1.0f;

        for (int i = 0; i < resizedImage.rows; ++i) {
            for (int j = 0; j < resizedImage.cols; ++j) {
                Vec4b color;
                if (hasAlpha) {
                    color = resizedImage.at<Vec4b>(i, j);
                } else {
                    Vec3b color3 = resizedImage.at<Vec3b>(i, j);
                    color = Vec4b(color3[0], color3[1], color3[2], 255); // Set alpha to 255 (fully opaque)
                }

                uchar b = color[0];
                uchar g = color[1];
                uchar r = color[2];
                alpha = color[3] / 255.0f; // Normalize alpha to 0-1

                if (alpha == 0) {
                    continue; // Skip fully transparent pixels
                }

                if (isDebugMode()) {
                    glBegin(GL_POINTS);
                    setColorFromRGB(r, g, b); // Assuming setColorFromRGB now also takes alpha
                    glVertex2i(j + location.X, i + location.Y);
                    glEnd();
                } else {
                    ushort color2 = (ushort) ((r << 11) | (g << 5) | b);
                    lcdDrawPixel(static_cast<uint16_t>(SCREEN_WIDTH - (location.X + x)), static_cast<uint16_t>(location.Y + y), color2);
                }
            }
        }
        drawScreen();
    }
};
#endif // !DISPLAY_H