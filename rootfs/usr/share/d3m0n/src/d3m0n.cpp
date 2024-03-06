#include <stdio.h>
#include "settings.h"
#include <display.cpp>
#include "application/appManager.cpp"
#include <string.h>
#include <utils.h>

int main(int argc, char *argv[])
{
    // init path
    string d3m0n_path = GetPath();

    system("clear");
    string command = (string)"rm -rf '"+d3m0n_path+(string)"/temp/' && mkdir '"+d3m0n_path+(string)"/temp/'";
    system(command.c_str());


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
	

	logn((string)"Splash Time: "+(string)getSetting("splash_time", GetConfig()), ConsoleColor::Reset);
	
	int screen_width    = 240;
    int screen_height   = 320;
	
    // init display
	display display1;
    display1.init(screen_width, screen_height);

    // load apps in memory
    appManager manager;
    Application gallery = manager.loadApp("./test/test.d3m0n");
    manager.runApp(gallery);

    callEvent(gallery, "Window.OnCreate");


    // loads apps launcher
    //    load topbar
    //    load background

    // load splashscreen
    // load apps in grid

    // hide splashscreen

	printf("\n");

    return 0;
}