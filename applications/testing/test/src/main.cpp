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

	printf("starting lua\n");

	uint32_t pid = exec("/programs/lua/lua", 0);
	printf("pid: %lu\n", pid);

	uint32_t code = kill(pid, 1234);
	printf("code: %lu\n", code);

	printf("MySimple Finished...\n");
	return 0;
}