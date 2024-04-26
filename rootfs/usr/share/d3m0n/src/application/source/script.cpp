#include "../../settings.h"
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <list>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = filesystem;
using namespace std;

class script
{
    public:
    static void callFunction(string name, list<string> args) {
        try {
            if(name == "log") { log(args.front(), LogStatus::Info); return; }
            else if(name == "logn") { logn(args.front(), ConsoleColor::get(args.back())); return; }
            else if(name == "print") { printf((args.front()+"\n").c_str()); return; }
            else if (name == "sleep") { this_thread::sleep_for(chrono::milliseconds(stoi(args.front()))); return;  }
        }
        catch(exception e)
        {
            log("Error while calling Function \""+name+"\": "+e.what(), LogStatus::Error);
        }
    }
};