#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"
#include "stdlib.h"

APP_INFO(
	"My Simple App",
	1.0,
	"4re5 group",
	"Official d3c application template",
	"icon.ico"
)

#include "windows/main_window.hpp"

extern "C" int	main(void)
{
	printf("MySimpleApp started\n");
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

	printf("MySimpleApp Finished...\n");
	return 0;
}
