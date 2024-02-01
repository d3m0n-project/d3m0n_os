#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class appManager {
	void loadApps()
	{
		
		Application newApp;
		newApp.name = "";
		newApp.icon = "";
		newApp.path = "";
		newApp.id = 0;
	}
	
	void runApp(int id)
	{}
	
	void closeApp(int id)
	{}
}

struct Application {
	char *name;
	char *icon;
	int id;
	char *path;
	
};