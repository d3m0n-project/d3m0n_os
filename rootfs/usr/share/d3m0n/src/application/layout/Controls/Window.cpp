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

using namespace std;


namespace layout {
class Window {
    public:
        display* display1;
        Window(display* display1, string key, string value) {
            if(key == "bg_color") {
                removeQuotes(value);
                display1->drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(value), true);
            }
        }
    };
};