#include "sys.h"

/*
	Entry point of the application. Do not edit.
*/

extern int	app_main(void);

void	_start(void)
{
	int ret = app_main();
	exit(ret);
	while (1);
}