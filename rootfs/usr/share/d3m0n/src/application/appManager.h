#ifndef APPMANAGER_H
#define APPMANAGER_H

#include "../settings.h"
#include <utils.h>
#include "layout/layoutHandler.h"
#include "source/sourceHandler.h"
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
#include <filesystem>
#include <random>
#include <vector>
#include <memory>



namespace fs = filesystem;
using namespace std;

namespace appManager {
    extern vector<string> appsPaths;
    extern vector<string> appsPackages;

    // appManager() {}
    // ~appManager() {}

    extern void unzipFile(string path, string output);
    

    extern Application loadApp(char* appPath);
    extern string GetAppPathByPackage(string package);
    extern void runApp(Application* app);
    extern string GetAppList();


    extern void launchApp(Application app);

    extern string mainScreenInit();
};

#endif // !APPMANAGER_H