#include <stdio.h>
#include "settings.h"
#include "display.hpp"
#include "application/appManager.cpp"
#include <string.h>
#include "utils.h"

int main(int argc, char *argv[])
{
    // init path
    string d3m0n_path = GetPath();

    system("clear");
    string command = (string)"rm -rf '"+d3m0n_path+(string)"/temp' && mkdir -p '"+d3m0n_path+(string)"/temp'";
    system(command.c_str());


    // init banner
	logn("      d3m0n OS     ", ConsoleColor::Yellow);
    logn("    by 4re5group     ", ConsoleColor::Yellow);
    printf("\n");
	
	//writeSetting("name1", "value2", "text.txt");
	
	
	
    // init display
	display display1;
    display1.init();

    // load apps in memory
    appManager manager;
    appManager::Application gallery = manager.loadApp("../apps/photo/gallery/gallery.d3m0n");
    manager.runApp(gallery);


    // loads apps launcher
    //    load topbar
    //    load background

    // load splashscreen
    // load apps in grid

    // hide splashscreen

	printf("\n");

    return 0;
}