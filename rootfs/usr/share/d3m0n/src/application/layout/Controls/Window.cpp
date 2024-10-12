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
#include <memory>
#include "display.h"


using namespace std;



namespace layout {
class Window {
    public:
        Window(string key, string value) {
            if(key == "bg_color") {
                removeQuotes(value);
                display::drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(value), true);
            }
        }
    };
};