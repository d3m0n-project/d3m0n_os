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
	MainWindow	window;
	window.draw();

	const char *argv2[] = {"/test.lua", 0};
	uint32_t pid = exec("/programs/bin/lua", argv2);
	//printf("lua pid: %lu\n", pid);

	while (pstate(pid) != PROC_ZOMBIE) // wait for process killing
		__asm__ volatile("nop"); // do nothing

	return 0;
}