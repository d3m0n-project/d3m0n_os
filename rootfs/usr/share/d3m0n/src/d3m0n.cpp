#include <stdio.h>
#include "settings.h"
#include <display.cpp>
#include "application/appManager.cpp"
#include <string.h>
#include <utils.h>

#define DEBUG 1

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
	

    int screen_width    = 240;
    int screen_height   = 320;

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
    display1.init(screen_width, screen_height);
    

    log("Splash loaded... Waiting "+(string)getSetting("splash_time", GetConfig())+"ms", LogStatus::Loading);
    // setting up splash image
    display1.drawImage(DataTypes::Point(0, 0), GetPath()+"/splash/splash.png", 20, 20, false);
    sleep(stoi(getSetting("splash_time", GetConfig()))/1000);


    // load apps in grid

    // hide splashscreen


    // load apps in memory
    appManager manager;
    Application gallery = manager.loadApp("./test/test.d3m0n");
    manager.runApp(&gallery);

    callEvent(&gallery, "Window.OnCreate");

    gallery.mainWindow->Update();

	printf("\n");

    return 0;
}