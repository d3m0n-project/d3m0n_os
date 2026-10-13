#include "stdio.hpp"

int	main(int argc, const char **argv)
{
	printf("\033[31margc: %i, argv: \033[0m", argc);
	for (int i=0; i<argc; i++)
	{
		printf("\033[31m'%s'   \033[0m", argv[i]);
	}
	printf("\n");
	return 1;
}