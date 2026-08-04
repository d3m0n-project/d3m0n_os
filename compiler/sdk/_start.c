#include "sys.h"

extern int	app_main(void);

void	_start(void)
{
	int ret = app_main();
	exit(ret);
}