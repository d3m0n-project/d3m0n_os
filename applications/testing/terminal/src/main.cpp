#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"
#include "stdlib.h"

APP_INFO(
	"d3m0n terminal",
	1.0,
	"4re5 group",
	"Basic terminal",
	"icon.ico"
)

#include "windows/main_window.hpp"

extern "C" int	main(void)
{
	printf("Terminal started\n");
	try
	{
		MainWindow	window;
		Application::run(window);
	}
	catch (exception &e)
	{
		fprintf(stderr, "\033[31mERROR\033[0m: An error occured while running app: \n");
		fprintf(stderr, "       >>> \033[31;1m%s\033[0m\n", e.what());
	}

	printf("Terminal Finished...\n");
	return 0;
}
