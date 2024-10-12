#include "appManager.h"

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
#include "display.h"


namespace appManager {
    vector<string> appsPaths;
    vector<string> appsPackages;

    void unzipFile(string path, string output) 
    {
        string command = (string)"unzip '"+path+(string)"' -d '"+output+(string)"' > /dev/null";
        system(command.c_str());
    }

    Application loadApp(char* appPath)
    {
        // loads app by unziping appPath file into /usr/share/d3m0n/temp/RANDOM_TOKEN
        const char* directory = GetPath().c_str(); // Directory to search .d3m0n files in
        // long randomToken = rand(); // Simple random token, consider a better random generation method
        random_device rd;
        mt19937 gen(rd()); // Mersenne Twister pseudo-random number generator
        // Define a distribution (e.g., uniform distribution between 0 and 99)
        uniform_int_distribution<long> dis(0, 1316134911);
        long randomToken = dis(gen);
        char temp_path[256];
        snprintf(temp_path, sizeof(temp_path), (GetPath()+(string)"/temp/%lu").c_str(), randomToken);

        log((string)"Unpacking '"+appPath+(string)"' ==> '"+temp_path+(string)"'", LogStatus::Info);
        if(!filesystem::exists(appPath))
        {
            log("Application not found.", LogStatus::Error);
            return Application{};
        }
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
        string package = (string)getSetting("package", path);
        // adds  app to list of apps
        appsPaths.push_back(temp_path);
        appsPackages.push_back(package);
        newApp.name = (string)getSetting("name", path);
        newApp.description = (string)getSetting("description", path);
        newApp.package = package;
        newApp.icon = (string)getSetting("icon", path);
        newApp.perms = (string)getSetting("perms", path);
        newApp.start_path = (string)getSetting("start_path", path);
        newApp.category = (string)getSetting("category", path);
        newApp.temp_path = (string)temp_path;
        return newApp;
    }
    string GetAppPathByPackage(string package) {
        int i=0;
        for (const string& str : appsPackages) {
            if(str == package) {
                return appsPaths.at(i);
            }
            i++;
        }
        return "not found";
    }
    void runApp(Application* app)
    {
        // runs app from Application struct. Gets path, runs /layout/main.layout then /src/main.src
        
        if(fs::exists((app->temp_path+(string)"/layouts/main.layout").c_str())) {
            log("Main layout found, Running...", LogStatus::Success);
            log((string)"   Name     => '"+app->name+(string)"'", LogStatus::Info);
            log((string)"   Desc     => '"+app->description+(string)"'", LogStatus::Info);
            log((string)"   Icon     => '"+app->icon+(string)"'", LogStatus::Info);
            log((string)"   Package  => '"+app->package+(string)"'", LogStatus::Info);
            log((string)"   Perms    => '"+app->perms+(string)"'", LogStatus::Info);
            log((string)"   Category => '"+app->category+(string)"'", LogStatus::Info);
            log((string)"   Path     => '"+app->temp_path+(string)"'", LogStatus::Info);
            log((string)"   Start    => '"+app->start_path+(string)"'", LogStatus::Info);
            logn("", ConsoleColor::Reset);
        } else {
            log("main.layout not found for app '"+app->name+"'/'"+app->package+"'", LogStatus::Error);
            return;
        }
        
        // init Window
        DataTypes::Window* window = new DataTypes::Window();
        window->size = DataTypes::Size(240, 320);
        app->mainWindow = window;
        app->windows.push_back(window);
        // starts displaying app
        layoutHandler::runLayout(app, app->temp_path+(string)"/layouts/main.layout");
        sourceHandler::runSource(app, app->temp_path+(string)"/src/main.src");
    }
    string GetAppList() {
        string to_return="";
        to_return+=logn("Current loaded apps", ConsoleColor::Blue);
        int i=0;
        for (const string& package : appsPackages) {
            string appFile = appsPaths.at(i)+"/app";
            to_return+=logn("        => "+(string)(getSetting("name", appFile.data()))+", package: "+package, ConsoleColor::Blue);
            i++;
        }
        return to_return;
        
    }
    void launchApp(Application app) {
        display::Clear();
        // log("CLICKED !!! "+app.name, LogStatus::Critical);
        runApp(&app);
        callEvent(&app, "Window.OnCreate");
        display::drawRect(0, 0, 100, 100, Colors::Red, true);
        app.mainWindow->Update();
        log("Successfully runned '"+app.name+"'!", LogStatus::Success);
    }
    string mainScreenInit() {
        string output="";
        printf("\n");
        output += log("Clearing loaded apps...", LogStatus::Loading);
        appsPackages.clear();
        appsPaths.clear();
        string commandClean = "rm -rf '"+GetPath()+"/temp/*'";
        system(commandClean.c_str());
        output += log("appManager is Loading mainscreen...", LogStatus::Loading);
        // display2->drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Colors::Green, true);
        // init main desktop screen
        string wallpaper = GetPath()+"/wallpapers/"+getSetting("wallpaper", (char*)GetConfig())+".png";
        display::drawImage(DataTypes::Point(0, 0), wallpaper, SCREEN_WIDTH, SCREEN_HEIGHT, false);
        output += log("DESKTOP initiated wallpaper => '"+wallpaper+"'", LogStatus::Success);

        output += logn("Loaded applications: ", ConsoleColor::Cyan);
        
        int app_count=0;
        Application temp;
        string appicon;
        int slot_x, slot_y = 0;
        int app_padding = 10;
        int appPerRow = 2;
        int appPerColumn = 4;
        int icon_size = SCREEN_WIDTH/appPerRow-20;
        slot_x = 0;
        
        for (const auto & entry : fs::directory_iterator(GetAppPath())) {
            if(endsWith(entry.path(), ".d3m0n")) {
                if(slot_x >= appPerRow) {
                    slot_y++;
                    slot_x=0;
                }
                
                
                output += logn("      => "+(string)entry.path(), ConsoleColor::Cyan);
                printf("      "); output += "      "; // for shell
                temp = loadApp(((string)entry.path()).data());
                output += logn("      x: "+to_string(slot_x)+", y: "+to_string(slot_y), ConsoleColor::Grey);
                
                if(contains(temp.icon, "/")) {
                    appicon=temp.icon;
                } else {
                    appicon=display::GetThemeIcon(temp.icon);
                }
                DataTypes::Point appRect = DataTypes::Point(slot_x*(icon_size+app_padding)+20, slot_y*(icon_size+app_padding));
                // display1->drawRect(appRect.X, appRect.Y, appRect.X+icon_size+10, appRect.Y+icon_size+10, Colors::Cyan, true);
                display::drawImage(appRect+10, appicon, icon_size-20, icon_size-20, false);
                display::drawText(20+slot_x*(icon_size+app_padding), slot_y*(icon_size+app_padding)+icon_size-8, 1, temp.name, false, Colors::White);
                
                display::registerEvent(UIEvent::OnClick, DataTypes::Rect(appRect.X, appRect.Y, appRect.X+icon_size, appRect.Y+icon_size+20), [](Application app) { launchApp(app); }, temp);
                slot_x++;
            }
        }
        
        
        
        output += "\n";
        printf("\n");
        return output;
    }
};