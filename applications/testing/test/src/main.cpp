#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"
#include "stdlib.h"

APP_INFO(
	"d3m0n test app",
	1.0,
	"4re5 group",
	"simple test app",
	"icon.ico"
)

#include "windows/main_window.hpp"

extern "C" int	main(void)
{
	printf("Test app started\n");
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

	printf("Test App Finished...\n");
	return 0;
}
