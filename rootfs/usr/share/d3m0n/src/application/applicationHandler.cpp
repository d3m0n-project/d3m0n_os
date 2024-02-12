#include "settingsFileHandler.h"
#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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

    void unzipFile(const char zipFilePath, const charoutputFolder) {
        int err = 0;
        zip_t zip = zip_open(zipFilePath, ZIP_RDONLY, &err);
        zip_extract(zip, outputFolder, NULL, NULL);
        zip_close(zip);
    }
    

    Application loadApp(char* appPath)
    {
        // loads app by unziping appPath file into /usr/share/d3m0n/temp/RANDOM_TOKEN

        DIRd;
        struct dirent dir;
        chardirectory = "/usr/share/d3m0n"; // Directory to search .d3m0n files in
        char temp_path[256];
        unsigned long randomToken = rand(); // Simple random token, consider a better random generation method
    
        snprintf(temp_path, sizeof(temp_path), "/usr/share/d3m0n/temp/%lu", randomToken);
    
        // Create the output directory
        mkdir(temp_path, 0777); // Note: Check return value in real code for errors
    
        d = opendir(directory);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (strstr(dir->d_name, ".d3m0n")) {
                    char fullPath[1024];
                    snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, dir->d_name);
                    unzipFile(fullPath, temp_path);
                }
            }
            closedir(d);
        }

        

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
