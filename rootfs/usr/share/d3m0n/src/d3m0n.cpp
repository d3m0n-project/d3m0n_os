#include <stdio.h>
#include "settings.h"
#include <display.cpp>
#include "application/appManager.cpp"
#include <string.h>
#include <utils.h>

#define COMMAND_HELP "d3m0n shell - help\n| command                       | permissions | description                      |\n|-------------------------------|-------------|----------------------------------|\n| help                          | none        | display this list                |\n| exit                          | none        | quit                             |\n| config [get/set] name (value) | admin       | config your phone                |\n| load <path>                   | none        | loads new app                    |\n| list                          | none        | list currently loaded apps       |\n| run <package>                 | none        | run app by package name          |\n| update                        | none        | update d3m0n OS                  |\n| fetch <package>               | none        | download package from repository |\n| clear                         | none        | clear console                    |"


void ShellHandle(appManager manager) {
    // loading shell
    log("d3m0n shell loaded, type 'help' to display main commands", LogStatus::Info);
    
    bool running=true;
    string command;
    string downloader_path = GetPath()+"/d3m0n_package_installer.sh";


    while(running) {
        printf("\e[1;32m[d3m0n shell]>\e[0m ");
        getline(cin, command);

        if(command != "") {
            if(delete_space(command)=="exit") running=false;
            else if(delete_space(command)=="clear") system("clear");
            else if(delete_space(command)=="help") logn(COMMAND_HELP, ConsoleColor::DarkGreen);
            else if(delete_space(command)=="list") appManager::GetAppList();
            else if(command == "clean") { 
                string remove_files = "rm -rf '"+downloader_path+"'";
                log("Cleaning scripts & applications...", LogStatus::Loading);
                manager.appsPackages = {};
                manager.appsPaths = {};
                system(remove_files.c_str());
                log("Done!", LogStatus::Success);
            }
            else if(startsWith(command, "update")) {
                string arg1 = Split(command, ' ', false);
                if(arg1 == "package_installer") {
                    log("Updating d3m0n package installer...", LogStatus::Loading);
                    int status = system(("rm -rf '"+downloader_path+"' && curl -o '"+downloader_path+"' 'https://raw.githubusercontent.com/d3m0n-project/d3m0n_os/main/rootfs/usr/share/applications/d3m0n_package_installer.sh' && chmod 777 '"+downloader_path+"'").c_str());
                    if(status != 0) { log("Update failed!", LogStatus::Error); }
                }
                else {
                    log("Usage 'update [all/package_installer]", LogStatus::Error);
                }
            } else if (startsWith(command, "load")) {
                string path = Split(command, ' ', false);
                manager.loadApp(path.data());
            } else if (startsWith(command, "run")) {
                string package = Split(command, ' ', false);
                Application newApp;
                newApp.temp_path = manager.GetAppPathByPackage(package.data());
                manager.runApp(&newApp);
            } else if (startsWith(command, "fetch")) {
                
                string package_or_name = Split(command, ' ', false);
                logn("   Downloading ==> "+package_or_name, ConsoleColor::Blue); 

                // downloads installer if missing
                if(!filesystem::exists(downloader_path))
                {
                    log("Downloading package installer script...", LogStatus::Loading);
                    system(("curl -o '"+downloader_path+"' 'https://raw.githubusercontent.com/d3m0n-project/d3m0n_os/main/rootfs/usr/share/applications/d3m0n_package_installer.sh'").c_str());
                }
                
                int status = system(("chmod 777 '"+downloader_path+"' &&  '"+downloader_path+"' '"+package_or_name+"'").c_str());
                // display run command
                logn("Run this app by loading it using '\e[1;33mload "+GetPath()+"/../applications/"+package_or_name+"\e[38;5;208m' \nand then run it using '\e[1;33mrun "+GetPath()+"../applications/"+package_or_name+"\e[38;5;208m'", ConsoleColor::Orange);
                // handle errors
                if(status != 0)
                {
                    log("Failed to fetch remote repository: are you offline?", LogStatus::Error);
                }
            }
            else { log("Invalid command '"+command+"' (help for list of available commands)", LogStatus::Error); }
        }

        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    // init path
    string d3m0n_path = GetPath();

    system("clear");
    string command = (string)"rm -rf '"+d3m0n_path+(string)"/temp/' && mkdir '"+d3m0n_path+(string)"/temp/'";
    system(command.c_str());

    // init display
	display display1;



    // loads apps launcher
    //    load topbar
    //    load background


    // init banner
    logn("    ██████╗ ██████╗ ███╗   ███╗ ██████╗ ███╗   ██╗", ConsoleColor::Reset);
    logn("    ██╔══██╗╚════██╗████╗ ████║██╔═████╗████╗  ██║", ConsoleColor::Reset);
    logn("    ██║  ██║ █████╔╝██╔████╔██║██║██╔██║██╔██╗ ██║", ConsoleColor::Reset);
    logn("    ██║  ██║ ╚═══██╗██║╚██╔╝██║████╔╝██║██║╚██╗██║", ConsoleColor::Reset);
    logn("    ██████╔╝██████╔╝██║ ╚═╝ ██║╚██████╔╝██║ ╚████║", ConsoleColor::Reset);
    logn("    ╚═════╝ ╚═════╝ ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝", ConsoleColor::Reset);
    logn("                  made by 4re5 group              ", ConsoleColor::Reset);
    logn("           the first hacking cellular phone       ", ConsoleColor::Reset);
    printf("\n");
	


    if(isDebugMode()) {
        string input;
        log("DEBUG mode is enabled wan't to continue? [y/N] ", LogStatus::Critical);
        cin >> input;
        if(input == "n" || input == "N")
        {
            return 0;
        }
    }


    // setup display
    display1.init(SCREEN_WIDTH, SCREEN_HEIGHT);
    

    log("Splash loaded... Waiting "+(string)getSetting("splash_time", GetConfig())+"ms", LogStatus::Loading);
    // setting up splash image
    display1.drawImage(DataTypes::Point(0, 0), GetPath()+"/splash/splash.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
    sleep(stoi(getSetting("splash_time", GetConfig()))/1000);

    // hide splashscreen
    display1.Clear();

    // load apps in memory
    appManager manager;
    Application gallery = manager.loadApp("./test/test.d3m0n");
    manager.runApp(&gallery);

    callEvent(&gallery, "Window.OnCreate");



    gallery.mainWindow->Update();
    log("Updated!", LogStatus::Success);

    ShellHandle(manager);


    // logn("Name: "+gallery.mainWindow->Controls.at(0)->name, ConsoleColor::Cyan);
    // logn("Width: "+to_string(gallery.mainWindow->Controls.at(0)->Width), ConsoleColor::Cyan);
    // logn("Hight: "+to_string(gallery.mainWindow->Controls.at(0)->Height), ConsoleColor::Cyan);
    // logn("Location.X: "+to_string(gallery.mainWindow->Controls.at(0)->Location.X), ConsoleColor::Cyan);
    // logn("Location.Y: "+to_string(gallery.mainWindow->Controls.at(0)->Location.Y), ConsoleColor::Cyan);
    // logn("BG_Color: "+to_string(gallery.mainWindow->Controls.at(0)->BackColor), ConsoleColor::Cyan);

    // load apps in grid


	printf("\n");

    return 0;
}