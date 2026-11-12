#include "stdio.hpp"
#include "app.hpp"

APP_INFO(
	"Test Game",
	1.0,
	"4re5 group",
	"Test 3d game that uses projection matrix",
	"icon.ico"
)

extern void	game_start(void);

extern "C" int	main(void)
{
	try
	{
		game_start();
	}
	catch (exception &e)
	{
		fprintf(stderr, "\033[31mERROR\033[0m: An error occured while running app: \n");
		fprintf(stderr, "       >>> \033[31;1m%s\033[0m\n", e.what());
	}

	return 0;
}
