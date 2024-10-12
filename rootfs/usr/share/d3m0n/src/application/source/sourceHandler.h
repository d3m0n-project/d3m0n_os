#ifndef SOURCEHANDLER_H
#define SOURCEHANDLER_H


#include "../../settings.h"
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
#include <list>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;
using namespace std;

// Forward declaration of Application class if it is not defined in settings.h or utils.h
class Application;

namespace sourceHandler {
    extern void parseSource(const string& path);
    
    extern void runSource(Application* app, const string& path);
};

void WriteSource(string path, string data);
void loadEvent(Application* app, string eventName, string eventCode);
void execute(Application* app, string line);
void callEvent(Application* app, string eventName);

namespace script
{
    extern void callFunction(string name, list<string> args);
};

#endif // SOURCEHANDLER_H
