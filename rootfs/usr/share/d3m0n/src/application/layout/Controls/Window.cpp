#include "../../../settings.h"
#include "../../../utils.h"
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
        Window() {
            printf("Window loaded\n");
        }
    };
};