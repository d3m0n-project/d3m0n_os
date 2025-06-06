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
#include <display.h>
#include <memory>


using namespace std;


namespace layout {
class TextBox : public Control {
    public:
        string text="";
        ushort BackColor = Colors::Black;
        ushort ForeColor = Colors::White;
        int FontSize = 10;
        bool Center = false;
        int FontPadding = 0;
        bool Bold = false;

        Control* Get() {
            return this;
        }

        void draw() override
        {
            int x1 = 240-this->Location.X;
            int y1 = this->Location.Y;

            int x2 = x1-this->size.Width;
            int y2 = y1+this->size.Height;

            display::drawText(x1, y1, this->FontSize, this->text, this->Bold, this->ForeColor);
            // font.WriteString(this->Text, this->FontSize, this->FontPadding, (ushort)x1, (ushort)y1, this->ForeColor, this->Center, (ushort)this->Size.Width, (ushort)this->Size.Height);
        }

            
        TextBox(string args) {
            // printf("TextBox loaded with arguments: %s\n", args.c_str());

        }
        static void parseAttributes(TextBox* ctrl, string key, string value) {
            GeneralAttributes::parseAttributes(ctrl->Get(), key, value);

            if(key == "content") {
                ctrl->text=value;
            } else if(key == "font_size") {
                ctrl->FontSize = stod(value);
            } else if(key == "text_align") {
                // TODO
                printf("TODO text_align\n");
            }
            else if(key == "bold") {
                // TODO
                printf("TODO bold\n");
                ctrl->Bold = true;
            }

        }

    };
};