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


namespace layout {
class Rect : public Control {
    public:
        ushort BackColor = Colors::Black;
        bool scroll = false;

        Control* Get() {
            return this;
        }

        void draw() override
        {
            int x1 = 240-this->Location.X;
            int y1 = this->Location.Y;

            int x2 = x1-this->size.Width;
            int y2 = y1+this->size.Height;

            
            display::drawRect(x2, y1, x1, y2, this->BackColor, true);
        }

            
        Rect(string args) {
            printf("Rect loaded with arguments: %s\n", args.c_str());

        }
        static void parseAttributes(Rect* ctrl, string key, string value) {
            GeneralAttributes::parseAttributes(ctrl->Get(), key, value);

            if(key == "scroll") {
                ctrl->scroll=(value=="true")?true:false;
            }

        }

    };
};