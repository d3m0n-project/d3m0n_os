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

	MainWindow	window;
	window.draw();
	
	int fd = open("test.txt", O_CREATE | O_WRITE);
	int fd2 = open("test2.txt", O_CREATE | O_WRITE);
	int fd3 = open("test3.txt", O_CREATE | O_WRITE);

	printf("test.txt: %i\n", fd);
	printf("test.txt: %i\n", fd2);
	printf("test.txt: %i\n", fd3);
	for (int i=0; i<3; i++)
		write(fd++, "Hello World!", 12);
	close(fd);

	printf("Finished...\n");
	return 0;
}