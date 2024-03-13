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
#include <cmath>

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

            // location="x, y";
            // location="[top/top_left/top_right/bottom/bottom_left/bottom_right/left/right/center]";

            // color="255, 255, 255";
            // bg_color="0, 0, 0";


            try {
                // name
                if(key == "name") {
                    ctrl->name =  value;
                } 
                // visible
                else if(key == "visible") {
                    ctrl->Visible = value == "true" ? true : false;
                }
                // width, height
                else if(startsWith(key,"width")) {
                    if(endsWith(value, "%")) {
                        float percent = stof(value.substr(0, value.length()))/100;
                        ctrl->Width = round(percent*SCREEN_WIDTH);
                    } else {
                        ctrl->Width = stoi(value);
                    }
                } else if(startsWith(key,"height")) {
                    if(endsWith(value, "%")) {
                        float percent = stof(value.substr(0, value.length()))/100;
                        ctrl->Height = round(percent*SCREEN_HEIGHT);
                    } else {
                        ctrl->Height = stoi(value);
                    }
                }
                // margin
                else if(startsWith(key,"margin")) {
                    if(key == "margin_top") { printf("TODO margin\n"); }
                    else if(key == "margin_left") { printf("TODO margin\n"); }
                    else if(key == "margin_right") { printf("TODO margin\n"); }
                    else if(key == "margin_bottom") { printf("TODO margin\n"); }
                } 
                // position
                else if(key == "location") {
                    if(value.find(",") != string::npos) {
                        string posX = delete_space(Split(value, ',', true));
                        string posY = delete_space(Split(value, ',', false));
                        if(isNumber(posX) && isNumber(posY)) {
                            ctrl->Location.X = stoi(posX);
                            ctrl->Location.Y = stoi(posY);
                            log("% >> X >> "+to_string(ctrl->Location.X), LogStatus::Error);
                            log("% >> Y >> "+to_string(ctrl->Location.Y), LogStatus::Error);
                        } else {
                            if(posX.find("%") != string::npos) {
                                float percent = stof(posX.substr(0, value.length()))/100;
                                ctrl->Location.X = round(percent*SCREEN_WIDTH);
                                log("% >> X >> "+to_string(ctrl->Location.X), LogStatus::Error);
                            }
                            if(posY.find("%") != string::npos) {
                                float percent = stof(posY.substr(0, value.length()))/100;
                                ctrl->Location.Y = round(percent*SCREEN_HEIGHT);
                                log("% >> Y >> "+to_string(ctrl->Location.Y), LogStatus::Error);
                            }
                        }
                    }
                } 
                // color, bg_color
                else if(key == "color") {
                    ctrl->ForeColor = GetColor(value);
                } else if(key == "bg_color") {
                    ctrl->BackColor = GetColor(value);
                } 
                // enabled
                else if(key == "enabled") {
                    ctrl->Enabled = value == "true" ? true : false;
                }
            }
            catch(exception e) {
                log("Error in layout '"+key+"': "+e.what(), LogStatus::Error);
            }

        }

    };
};