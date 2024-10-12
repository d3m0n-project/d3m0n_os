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
using namespace DataTypes;



namespace layout {
class Text : public Control {
    public:
        string text = "";
        ushort BackColor = Colors::White;
        ushort ForeColor = Colors::Black;
        bool Bold = false;
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

            // Get text width and height
            int text_width = this->FontSize*this->text.length();
            int text_height = this->FontSize*2;

            
            // sets width&height to text size if not changed
            // log("Text size not def", LogStatus::Critical);
            if(this->size.Width == 100 && this->size.Height == 50) {
                log("SIZE NOT DEF => OVERRIDE", LogStatus::Critical);
                this->size = DataTypes::Size(text_width, text_height);
            }
            else {
                // removes extra chars that are out of rect
                if(this->size.Width > text_width) {
                    log("TOO LONG FOR TEXT", LogStatus::Critical);
                    logn(this->text, ConsoleColor::DarkGreen);
                    this->text.substr(0, this->text.length()-(this->size.Width-text_width)/FontSize);
                    logn(this->text, ConsoleColor::DarkGreen);
                }
            }

            if(isDebugMode()) {
                this->Location.Y += this->size.Height;
            }


            // display BackGround if not default
            // log("Disabled backcolor: Text", LogStatus::Critical);
            if(this->BackColor != Colors::White) {
                display::drawRect(x1, y1, x2, y2, this->BackColor, true);
            }
            display::drawText(this->Location.X, this->Location.Y, this->FontSize, this->text, this->Bold, this->ForeColor);
            // font.WriteString(this.Text, this.FontSize, this.FontPadding, (ushort)x1, (ushort)y1, this.ForeColor, this.Center, (ushort)this.Size.Width, (ushort)this.Size.Height);
        }

            
        Text(string args) {
            // printf("Text loaded with arguments: %s\n", args.c_str());

        }
        static void parseAttributes(Text* ctrl, string key, string value) {
            ctrl = static_cast<Text*>(GeneralAttributes::parseAttributes(ctrl, key, value));

            if(key == "content") {
                removeQuotes(value);
                ctrl->text = value;
            } else if(key == "font_size") {
                // TODO
                try {
                    removeQuotes(value);
                    ctrl->FontSize = stoi(value);
                } catch(exception) {
                    log("Bad FontSize (should be a int)", LogStatus::Error);
                }
            }
            else if(key == "text_align") {
                // TODO
                printf("TODO text_align\n");
                ctrl->Center = true;
            }
            else if(key == "bold") {
                // TODO
                printf("TODO bold\n");
                ctrl->Bold = true;
            }

        }

    };
};