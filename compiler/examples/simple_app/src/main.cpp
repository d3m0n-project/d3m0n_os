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

	uint32_t	*test = (uint32_t *)0x4000;
	*test = 424242;

	printf("test = %lu\n", *test);

	MainWindow	window;
	window.draw();

	printf("Finished...\n");
	return 0;
}