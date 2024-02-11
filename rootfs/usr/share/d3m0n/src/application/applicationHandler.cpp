#include "settingsFileHandler.h"


class applicationHandler {
    struct Application
    {
        char* name;
        char* package;
        char* icon;
        char* perms;
        char* start_path;
        char* category;
        char* temp_path;
    };

    Application loadApp(char* appPath)
    {
        // loads app by unziping appPath file into /usr/share/d3m0n/temp/RANDOM_TOKEN
        char* temp_path = "Generated app's temp path goes here";


        Application newApp;
        newApp.name = getSetting("name", temp_path+"/app");
        newApp.package = getSetting("package", temp_path+"/app");
        newApp.icon = getSetting("icon", temp_path+"/app");
        newApp.perms = getSetting("perms", temp_path+"/app");
        newApp.start_path = getSetting("start_path", temp_path+"/app");
        newApp.category = getSetting("category", temp_path+"/app");
        newApp.temp_path = temp_path;

        return newApp;
    }
    void runApp(Application app)
    {
        // runs app from Application struct. Gets path, runs /layout/main.layout then /src/main.src
        newApp.name = getSetting("name", appPath+"/app");
        newApp.package = getSetting("package", appPath+"/app");
        newApp.icon = getSetting("icon", appPath+"/app");
        newApp.perms = getSetting("perms", appPath+"/app");
        newApp.start_path = getSetting("start_path", appPath+"/app");
        newApp.category = getSetting("category", appPath+"/app");
        newApp.temp_path = temp_path;
    }
}