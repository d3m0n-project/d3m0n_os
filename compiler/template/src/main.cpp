#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"

APP_INFO(
	"My Simple App",
	1.0,
	"My Company",
	"A simple application",
	"icon.ico"
)

#include "windows/main_window.hpp"


extern "C" int	main(void)
{
	printf("MySimple app started\n");

	MainWindow	window;
	window.draw();

	printf("MySimple app Finished...\n");
	return 0;
}