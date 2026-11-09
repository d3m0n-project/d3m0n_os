#include "stdio.hpp"

int	main(int argc, const char **argv)
{
	printf("argc: %i, argv:\n", argc);
	for (int i=0; i<argc; i++)
		printf("'%s': %p\n", argv[i], argv[i]);
	printf("\n");

	int c = getc(stdin);
	printf("pressed %c\n", c);

	printf("done\n");
	return 1;
}