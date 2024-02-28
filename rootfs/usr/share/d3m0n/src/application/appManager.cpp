#include "../settings.h"
#include "../utils.h"
#include "layout/layoutHandler.cpp"
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

namespace fs = filesystem;
using namespace std;

class appManager {
public:
	struct Application
    {
        string name;
        string description;
        string package;
        string icon;
        string perms;
        string start_path;
        string category;
        string temp_path;
    };

    void unzipFile(string path, string output) 
    {
        string command = (string)"unzip '"+path+(string)"' -d '"+output+(string)"' > /dev/null";
        system(command.c_str());
    }
    

    Application loadApp(char* appPath)
    {
        // loads app by unziping appPath file into /usr/share/d3m0n/temp/RANDOM_TOKEN

        const char* directory = GetPath().c_str(); // Directory to search .d3m0n files in
        unsigned long randomToken = rand(); // Simple random token, consider a better random generation method
    
        char temp_path[256];
        snprintf(temp_path, sizeof(temp_path), (GetPath()+(string)"/temp/%lu").c_str(), randomToken);
    
        log((string)"Unpacking '"+appPath+(string)"' ==> '"+temp_path+(string)"'", LogStatus::Info);


        // Create the output directory
        mkdir(temp_path, 0777); // Note: Check return value in real code for errors
    
        unzipFile(appPath, temp_path);

        // TODO: add unzip
        // for (const auto & entry : fs::directory_iterator(directory)) {
            
        //     string test = (string)entry.path();
        //     printf(test.c_str());
        // }

        

        Application newApp;
        //logn((string)temp_path, ConsoleColor::Red);
        string path2 = (string)temp_path+(string)"/app";
        char* path = new char[path2.length() + 1];
        strcpy(path, path2.c_str());



        //logn(path, ConsoleColor::Red);
        newApp.name = (string)getSetting("name", path);
        newApp.description = (string)getSetting("description", path);
        newApp.package = (string)getSetting("package", path);
        newApp.icon = (string)getSetting("icon", path);
        newApp.perms = (string)getSetting("perms", path);
        newApp.start_path = (string)getSetting("start_path", path);
        newApp.category = (string)getSetting("category", path);
        newApp.temp_path = (string)temp_path;

        return newApp;
    }
    void runApp(Application app)
    {
        // runs app from Application struct. Gets path, runs /layout/main.layout then /src/main.src
        
        if(fs::exists((app.temp_path+(string)"/layouts/main.layout").c_str())) {
            log("Main layout found, Running...", LogStatus::Success);
            log((string)"   Name     => '"+app.name+(string)"'", LogStatus::Info);
            log((string)"   Desc     => '"+app.description+(string)"'", LogStatus::Info);
            log((string)"   Icon     => '"+app.icon+(string)"'", LogStatus::Info);
            log((string)"   Package  => '"+app.package+(string)"'", LogStatus::Info);
            log((string)"   Perms    => '"+app.perms+(string)"'", LogStatus::Info);
            log((string)"   Category => '"+app.category+(string)"'", LogStatus::Info);
            log((string)"   Path     => '"+app.temp_path+(string)"'", LogStatus::Info);
            log((string)"   Start    => '"+app.start_path+(string)"'", LogStatus::Info);
            logn("", ConsoleColor::Reset);
        } else {
            log("main.layout not found for app '"+app.name+"'/'"+app.package+"'", LogStatus::Error);
            return;
        }
        
        // starts displaying app
        layoutHandler layout;

        layout.runLayout(app.temp_path+(string)"/layouts/main.layout");



    }
};