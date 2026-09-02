#include "sys.h"
#include "stdio.hpp"

/*
	d3m0n os
	Entry point of the application. Do not edit.
*/

#ifndef DEBUG
#define DEBUG	0
#endif


extern "C" void	abort(void)
{
	for (;;)
		;
}

extern "C" int	app_main(void);

extern "C" void	_start(void)
{
	try
	{
		#if DEBUG == 1
		write(1, "DEBUG: App Started\n", 19);
		#endif

		int ret = app_main();
		exit(ret);
		while (1);
	}
	catch (AppException& e)
	{
		printf("\033[31mERROR\033[0m: %s\n", e.what());
	}
}