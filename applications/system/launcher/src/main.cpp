#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"
#include "stdlib.h"

APP_INFO(
	"d3m0n default app launcher",
	1.1,
	"4re5 group",
	"Official default app launcher, homescreen application",
	"icon.ico"
)

#include "windows/main_window.hpp"

static event_t				pending_event;
static volatile uint32_t	event_pending;

static void	launcher_event_thread(void)
{
	while (1)
	{
		if (event_pending)
			continue;
		event_t event = {.type=EVENT_UNKNOWN, .timestamp=0, .x=0, .y=0};
		
		if (evtpop(&event))
		{
			pending_event = event;
			__asm__ volatile("" ::: "memory");
			event_pending = 1;
		}
	}
}

extern "C" int	main(void)
{
	printf("Launcher started\n");
	try
	{
		MainWindow	window;
		window.draw(); 
		thread_t events = thread_create(launcher_event_thread);
		if (!events)
		{
			fprintf(stderr, "Could not start launcher event thread\n");
			while (1)
			{
				event_t event = {.type=EVENT_UNKNOWN, .timestamp=0, .x=0, .y=0};
				if (evtpop(&event) && window.handle_event(&event))
					window.draw();
			}
		}
		while (1)
		{
			if (event_pending)
			{
				__asm__ volatile("" ::: "memory");
				event_t event = pending_event;
				event_pending = 0;
				__asm__ volatile("" ::: "memory");
				if (window.handle_event(&event))
					window.draw();
			}
		}
	}
	catch (exception &e)
	{
		fprintf(stderr, "\033[31mERROR\033[0m: An error occured while running app: \n");
		fprintf(stderr, "       >>> \033[31;1m%s\033[0m\n", e.what());
	}

	printf("Launcher Finished...\n");
	return 0;
}
