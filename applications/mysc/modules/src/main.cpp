#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"

APP_INFO(
	"Modules",
	1.0,
	"4re5 group",
	"Browse d3m0n OS system modules",
	"icon.ico"
)

#include "windows/main_window.hpp"

extern "C" int	main(void)
{
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

	return 0;
}
