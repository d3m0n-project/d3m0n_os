#include "memory.h"
#include "sys.h"

int	app_main(void)
{
	print("started\n");
	char *test = malloc(614400);
	if (!test)
		print("fail\n");
	else
		print("success\n");
	print("stopped\n");
	return 0;
}