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
	try
	{
		printf("hello\n");
		write(1, "A\n", 2);
		printf("hello %d\n", 42);
		write(1, "B\n", 2);
		printf("hello %s\n", "world");
		write(1, "C\n", 2);
		printf("hello %c\n", 'X');
		write(1, "D\n", 2);
		printf("100%%\n");
		write(1, "E\n", 2);
	}
	catch (const exception& e)
	{
		write(1, "except\n", 7);
	}
	return (0);


	printf("app_main\n");
	write(1, "printf finished\n", 17);
	MainWindow window;
	write(1, "printf finished\n", 17);
	window.draw();

	printf("finished\n");
	return 0;
}