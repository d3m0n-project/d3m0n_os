#include "sys.h"

/*
	d3m0n os
	Entry point of the application. Do not edit.
*/

#ifndef DEBUG
#define DEBUG	0
#endif


void	abort(void)
{
	for (;;)
		;
}

extern int	app_main(void);

void	_start(void)
{
	#if DEBUG == 1
	write(1, "DEBUG: App Started\n", 19);
	#endif

	int ret = app_main();
	exit(ret);
	while (1);
}