#include "sys.h"

int	app_main(void)
{
	char	test[4096];

	print("LS Result:\n");

	getdents("/", test, 4096);
	print(test);

	return 0;
}