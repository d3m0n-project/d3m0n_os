#include "sys.h"

extern int main(int argc, char **argv);

void _start(int argc, char **argv)
{
	// TODO: fix system crash if no exit is called
	exit(main(argc, argv));
}
