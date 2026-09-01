#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"

APP_INFO(
	"My Simple App",
	1.0,
	"4re5 group",
	"A simple application",
	"icon.ico"
)

#include "windows/main_window.hpp"


extern "C" int	app_main(void)
{
	printf("MySimple app started\n");

	try
	{
		MainWindow	window;
		window.draw();
	}
	catch (AppException& e)
	{
		printf("\033[31mERROR\033[0m: %s\n", e.what());
	}

	printf("Finished...\n");
	return 0;
}