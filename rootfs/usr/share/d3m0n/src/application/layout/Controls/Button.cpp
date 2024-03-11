#include "../../../settings.h"
#include <utils.h>
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
#include <display.cpp>

using namespace std;
using namespace DataTypes;

namespace layout {
class Button : public Control {
    public:
        string text="";
        Bitmap image;
        ushort BackColor = Colors::Black;
        ushort ForeColor = Colors::White;
        int FontSize = 10;
        bool Center = false;
        int FontPadding = 0;
        DataTypes::Size defaultSize = DataTypes::Size(100, 50);

        Control* Get() {
            return this;
        }

        void draw() override
        {
            int x1 = 240-this->Location.X;
            int y1 = this->Location.Y;

            int x2 = x1-this->size.Width;
            int y2 = y1+this->size.Height;

            // check if size not changed
            if(defaultSize.Width == this->size.Width && defaultSize.Height == this->size.Height)
            {
                // then define width with text size
                this->size.Width = this->text.length()*(this->FontSize+this->FontPadding);
            }

            
            // ili9340.lcdDrawFillRect((ushort)(x2-Padding), (ushort)(y1-Padding), (ushort)(x1+Padding), (ushort)(y2+Padding), this->BackColor);
            // //                 text  size pad  x   y  center
            // font.WriteString(this->Text, this->FontSize, this->FontPadding, (ushort)x1, (ushort)y1, this->ForeColor, this->Center, (ushort)this->Size.Width, (ushort)this->Size.Height);

            // // debug top-right
            // ili9340.lcdDrawPixel((ushort)x2, (ushort)y1, Colors.Yellow);
            // // and bottom-left pixels
            // ili9340.lcdDrawPixel((ushort)x1, (ushort)y2, Colors.Green);
        }

            
        Button(string args) {
            printf("Button loaded with arguments: %s\n", args.c_str());

        }
        static void parseAttributes(Button* ctrl, string key, string value) {
            GeneralAttributes::parseAttributes(ctrl->Get(), key, value);

            if(key == "content") {
                ctrl->text=value;
            } else if(key == "type") {
                // TODO
                printf("TODO type\n");
            } else if(key == "font_size") {
                ctrl->FontSize = stod(value);
            } else if(key == "text_align") {
                // TODO
                printf("TODO text_align\n");
            } else if(key == "image") {
                // TODO
                printf("TODO image\n");
            }

        }

    };
};