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
class Image : public Control {
    public:
        string src;
        ushort BackColor = Colors::Black;
        bool reverse = false;

        Control* Get() {
            return this;
        }

        DataTypes::Rect getRect()
        {
            int x1 = 240-this->Location.X;
            int y1 = this->Location.Y;

            int x2 = x1-this->size.Width;
            int y2 = y1+this->size.Height;

            x1-=this->size.Width;
            x2-=this->size.Width;

            return DataTypes::Rect(x2, y1, x1, y2);
        }

        void draw() override
        {
            if(isDebugMode()) return;

            display::drawImage(DataTypes::Point(this->Location.X, this->Location.Y), this->src, this->size.Width, this->size.Height, false);
            log("Drawing image!", LogStatus::Warning);
        }

            
        Image(string args) {
            printf("Image loaded with arguments: %s\n", args.c_str());

        }
        static void parseAttributes(Image* ctrl, string key, string value) {
            // logn(to_string(ctrl->ForeColor)+" <= color", ConsoleColor::Yellow);
            ctrl = static_cast<Image*>(GeneralAttributes::parseAttributes(ctrl, key, value));
            // logn(to_string(ctrl->ForeColor)+" <= color", ConsoleColor::Yellow);

            if(key == "src") {
                // TODO
                removeQuotes(value);
                if(startsWith(value, "http")){
                    string command = "curl -o '"+GetPath()+"/temp/image.png' '"+value+"'";
                    system(command.c_str());
                    ctrl->src=GetPath()+"/temp/image.png";
                }
                else if(filesystem::exists(value)) {
                    ctrl->src=value;
                }
                else {
                    ctrl->src=display::GetThemeIcon(value);
                }
                
            } else if(key == "mode") {
                // TODO
                printf("TODO mode\n");
                // ctrl->FontSize = 0;
            }

        }

    };
};