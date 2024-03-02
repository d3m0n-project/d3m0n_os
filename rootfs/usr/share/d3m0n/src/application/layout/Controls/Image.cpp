#include "../../../settings.h"
#include "../../../utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "Type.h"
#include "../../../display.hpp"

using namespace std;


namespace layout {
class Image : public Control {
    public:
        Bitmap src;
        ushort BackColor = Colors::Black;
        bool reverse = false;

        Control& Get() {
            return *this;
        }

        Rect getRect()
        {
            int x1 = 240-this->Location.X;
            int y1 = this->Location.Y;

            int x2 = x1-this->size.Width;
            int y2 = y1+this->size.Height;

            x1-=this->size.Width;
            x2-=this->size.Width;

            return Rect(x2, y1, x1, y2);
        }

        void draw()
        {
            int x1 = 240-this->Location.X;
            int y1 = this->Location.Y;

            int x2 = x1-this->size.Width;
            int y2 = y1+this->size.Height;

            // Bitmap bmp2 = (Bitmap)Img;
            // resize to width 
            // Bitmap bmp = utils.ResizeBitmap(bmp2, this->size.Width, this->size.Height);
            cout << "Bitmap points:" << endl;
            for (const auto& point : src.points) {
                cout << "Point (" << point.x << ", " << point.y << ") - Color: " << hex << point.color << endl;
            }
            // for (int ii = 0; ii < bmp.Width; ii++)
            // {
            //     for (int jj = 0; jj < bmp.Height; jj++)
            //     {
            //         Color pixelColor = bmp.GetPixel(ii, jj);
            //         if(pixelColor.A > 0) {
            //             ili9340.lcdDrawPixel((ushort)(x1-ii), (ushort)(y1+jj), Colors.getColor(pixelColor));
            //         }
            //     }
            // }

            // // debug top-right
            // ili9340.lcdDrawPixel((ushort)x2, (ushort)y1, Colors.Red);
            // // and bottom-left pixels
            // ili9340.lcdDrawPixel((ushort)x1, (ushort)y2, Colors.Green);
        }

            
        Image(string args) {
            printf("Image loaded with arguments: %s\n", args.c_str());

        }
        static void parseAttributes(Image* ctrl, string key, string value) {
            // logn(to_string(ctrl->ForeColor)+" <= color", ConsoleColor::Yellow);
            GeneralAttributes::parseAttributes(&ctrl->Get(), key, value);
            // logn(to_string(ctrl->ForeColor)+" <= color", ConsoleColor::Yellow);

            if(key == "src") {
                // TODO
                printf("TODO src\n");
                ctrl->src.points.push_back({0, 0, Colors::Red}); // Example: Red color at (0, 0)
                ctrl->src.points.push_back({1, 1, Colors::Green}); // Example: Green color at (1, 1)
                ctrl->src.points.push_back({2, 2, Colors::Blue}); // Example: Blue color at (2, 2)
            } else if(key == "mode") {
                // TODO
                printf("TODO mode\n");
                // ctrl->FontSize = 0;
            }

        }

    };
};