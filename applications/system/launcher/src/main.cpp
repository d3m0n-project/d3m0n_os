#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"
#include "stdlib.h"

APP_INFO(
	"d3m0n default app launcher",
	1.1,
	"4re5 group",
	"Official default app launcher, homescreen application",
	"icon.ico"
)

#include "windows/main_window.hpp"


extern "C" int	main(void)
{
	printf("Launcher started\n");

	try
	{
		MainWindow	window;
		window.draw();
	}
	catch(exception &e)
	{
		printf("\033[31mERROR\033[0m: An error occured while running app: \n       >>> \033[31;1m%s\033[0m\n", e.what());
	}

	printf("Launcher Finished...\n");
	return 0;
}