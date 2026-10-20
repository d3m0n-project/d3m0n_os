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

void	sleep(uint32_t ms)
{
	time_t t = time(NULL);
	while (time(NULL) < t + ms)
		__asm__ volatile("nop");
}

extern "C" int	main(void)
{
	MainWindow	window;
	window.draw();

	//int i=0;
	//while (1)
	//{
	//	event_t e = (event_t){};
	//	e.type=EVENT_UNKNOWN;
	//	int ret = evtpop(&e);
	//	if (ret && EVENT_IS_MOUSE(e.type))
	//		printf("click triggered at (%i, %i): type is mouse %lu at %lus\n", e.x, e.y, e.type, e.timestamp);
		
	//	i++;
	//	if (i % 1000 == 999)
	//	{
	//		window.draw();
	//		i = 0;
	//	}
	//}

	//const char *argv2[] = {"/test.lua", 0};
	//uint32_t pid = exec("/programs/bin/lua", argv2);

	const char *argv2[] = {"/test.py", 0};
	uint32_t pid = exec("/programs/bin/python3", argv2);

	while (pstate(pid) != PROC_ZOMBIE) // wait for process killing
		__asm__ volatile("nop"); // do nothing

	return 0;
}