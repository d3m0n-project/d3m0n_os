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

extern "C" int	app_main(void)
{
	printf("Hello World!\n");
	return 0;
}