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
        static T parseAttributes(T ctrl, string key, string value) {
            removeQuotes(value);
            // parent="ControlName";

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
                        ctrl->size.Width = round(percent*SCREEN_WIDTH);
                    } else {
                        ctrl->size.Width = stoi(value);
                    }
                } else if(startsWith(key,"height")) {
                    if(endsWith(value, "%")) {
                        float percent = stof(value.substr(0, value.length()))/100;
                        ctrl->size.Height = round(percent*SCREEN_HEIGHT);
                    } else {
                        ctrl->size.Height = stoi(value);
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
                        
                        // enable percentage ratio for X
                        if(posX.find("%") != string::npos) {
                            float percent = stof(posX.substr(0, value.length()))/100;
                            ctrl->Location.X = round(percent*SCREEN_WIDTH);
                        }
                        else {
                            ctrl->Location.X = stoi(posX);
                        }
                        // do the same but for Y
                        if(posY.find("%") != string::npos) {
                            float percent = stof(posY.substr(0, value.length()))/100;
                            ctrl->Location.Y = round(percent*SCREEN_HEIGHT);
                        }
                        else {
                            ctrl->Location.Y = stoi(posY);
                        }
                    }
                    else {
                        // handle arbitrary locations
                        if(value == "top") {
                            ctrl->Location.X = (SCREEN_WIDTH/2)-(ctrl->size.Width/2);
                            ctrl->Location.Y = 0;
                        } else if(value == "top_left") { 
                            ctrl->Location.X = 0;
                            ctrl->Location.Y = 0; 
                        } else if(value == "top_right") { 
                            ctrl->Location.X = SCREEN_WIDTH-ctrl->size.Width; 
                            ctrl->Location.Y = 0; 
                        } else if(value == "bottom") { 
                            ctrl->Location.X = SCREEN_WIDTH/2-ctrl->size.Width/2; 
                            ctrl->Location.Y = SCREEN_HEIGHT-ctrl->size.Height;
                        } else if(value == "bottom_left") { 
                            ctrl->Location.X = 0; 
                            ctrl->Location.Y = SCREEN_HEIGHT-ctrl->size.Height;
                        } else if(value == "bottom_right") { 
                            ctrl->Location.X = SCREEN_WIDTH-ctrl->size.Width; 
                            ctrl->Location.Y = SCREEN_HEIGHT-ctrl->size.Height;
                        } else if(value == "left") { 
                            ctrl->Location.X = 0; 
                            ctrl->Location.Y = (SCREEN_HEIGHT/2)-(ctrl->size.Height/2);
                        } else if(value == "right") { 
                            ctrl->Location.X = SCREEN_WIDTH-ctrl->size.Width; 
                            ctrl->Location.Y = (SCREEN_HEIGHT/2)-(ctrl->size.Height/2);
                        } else if(value == "center") { 
                            ctrl->Location.X = (SCREEN_WIDTH/2)-(ctrl->size.Width/2); 
                            ctrl->Location.Y = (SCREEN_HEIGHT/2)-(ctrl->size.Height/2);
                        }
                    }
                } 
                // margins
                else if(key == "margin_top") {
                    if(value.find("%") != string::npos) {
                        float percent = stof(value.substr(0, value.length()))/100;
                        ctrl->Location.Y += round(percent*SCREEN_HEIGHT);
                    } else {
                        ctrl->Location.Y += stoi(value);
                    }
                }
                else if(key == "margin_left") {
                    if(value.find("%") != string::npos) {
                        float percent = stof(value.substr(0, value.length()))/100;
                        ctrl->Location.X += round(percent*SCREEN_HEIGHT);
                    } else {
                        ctrl->Location.X += stoi(value);
                    }
                }
                else if(key == "margin_right") {
                    if(value.find("%") != string::npos) {
                        float percent = stof(value.substr(0, value.length()))/100;
                        ctrl->Location.X -= round(percent*SCREEN_HEIGHT);
                    } else {
                        ctrl->Location.X -= stoi(value);
                    }
                }
                else if(key == "margin_bottom") {
                    if(value.find("%") != string::npos) {
                        float percent = stof(value.substr(0, value.length()))/100;
                        ctrl->Location.Y -= round(percent*SCREEN_HEIGHT);
                    } else {
                        ctrl->Location.Y -= stoi(value);
                    }
                }
                // color, bg_color
                else if(key == "color") {
                    ctrl->ForeColor = (ushort)GetColor(value);
                    logn("Set ForeColor to '"+value+"' => "+to_string(ctrl->ForeColor), ConsoleColor::Red);
                } else if(key == "bg_color") {
                    ctrl->BackColor = (ushort)GetColor(value);
                    logn("Set BackColor to '"+value+"' => "+to_string(ctrl->BackColor), ConsoleColor::Red);
                } 
                // enabled
                else if(key == "enabled") {
                    ctrl->Enabled = value == "true" ? true : false;
                }
            }
            catch(exception e) {
                log("Error in layout '"+key+"': "+e.what(), LogStatus::Error);
            }

            return ctrl;

        }

    };
};