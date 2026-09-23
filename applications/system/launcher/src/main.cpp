#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"

APP_INFO(
	"d3m0n default app launcher",
	1.0,
	"4re5 group",
	"Official default app launcher, homescreen application",
	"icon.ico"
)

#include "windows/main_window.hpp"


extern "C" int	app_main(void)
{
	printf("Launcher started\n");

	MainWindow	window;
	window.draw();

	printf("Launcher Finished...\n");
	return 0;
}