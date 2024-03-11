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
class Text : public Control {
    public:
        string text = "";
        ushort BackColor = Colors::Black;
        ushort ForeColor = Colors::White;
        int FontSize = 10;
        bool Center = false;
        int FontPadding = 0;

        Control* Get() {
            return this;
        }

        void draw() override
        {
            int x1 = 240-this->Location.X;
            int y1 = this->Location.Y;

            int x2 = x1-this->size.Width;
            int y2 = y1+this->size.Height;


            // font.WriteString(this.Text, this.FontSize, this.FontPadding, (ushort)x1, (ushort)y1, this.ForeColor, this.Center, (ushort)this.Size.Width, (ushort)this.Size.Height);
        }

            
        Text(string args) {
            printf("Text loaded with arguments: %s\n", args.c_str());

        }
        static void parseAttributes(Text* ctrl, string key, string value) {
            // logn(to_string(ctrl->ForeColor)+" <= color", ConsoleColor::Yellow);
            GeneralAttributes::parseAttributes(ctrl->Get(), key, value);
            // logn(to_string(ctrl->ForeColor)+" <= color", ConsoleColor::Yellow);

            if(key == "content") {
                ctrl->text =  value;
            } else if(key == "font_size") {
                // TODO
                printf("TODO font_size\n");
                ctrl->FontSize = 0;
            }
            else if(key == "text_align") {
                // TODO
                printf("TODO text_align\n");
                ctrl->Center = true;
            }

        }

    };
};