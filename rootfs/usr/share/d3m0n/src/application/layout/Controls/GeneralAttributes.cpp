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

using namespace std;


namespace layout {
class GeneralAttributes {
    public:
        template<typename T>
        static void parseAttributes(T* ctrl, string key, string value) {
            removeQuotes(value);
            // enabled="[true/false]";
            // parent="ControlName";

            // margin_top="10";
            // margin_left="20";
            // margin_right="5";
            // margin_bottom="25";

            // position="x, y";
            // position="[top/top_left/top_right/bottom/bottom_left/bottom_right/left/right/center]";

            // color="white";
            // color="255, 255, 255";

            // bg_color="black";
            // bg_color="0, 0, 0";

            // width="20";
            // width="10%";

            // height="10";
            // width="5%";
            try {
                if(key == "name") {
                    ctrl->name =  value;
                } else if(key == "visible") {
                    ctrl->Visible = value == "true" ? true : false;
                }
                else if(startsWith(key,"margin")) {
                    if(key == "margin_top") { printf("TODO margin\n"); }
                    else if(key == "margin_left") { printf("TODO margin\n"); }
                    else if(key == "margin_right") { printf("TODO margin\n"); }
                    else if(key == "margin_bottom") { printf("TODO margin\n"); }
                } else if(key == "position") {
                    // if(value)
                    printf("TODO position\n");
                } else if(key == "color") {
                    log("SET COLOR to '"+value+"'", LogStatus::Critical);
                    ctrl->ForeColor = GetColor(value);
                } else if(key == "bg_color") {
                    unsigned long ulongValue = std::stoul(value);
                    if (ulongValue > std::numeric_limits<unsigned short>::max()) {
                        throw std::out_of_range("Value exceeds range of unsigned short");
                    }
                    ctrl->BackColor = static_cast<unsigned short>(ulongValue);
                } else if(key == "visible") {

                }
            }
            catch(exception e) {
                log("Error in layout '"+key+"': "+e.what(), LogStatus::Error);
            }

        }

    };
};