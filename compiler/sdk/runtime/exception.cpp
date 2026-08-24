#include "stdio.hpp"

namespace std
{
	void	__throw_out_of_range_fmt(const char *fmt, ...)
	{
		printf("ERROR: Caught a out of range: '%s'\n", fmt);
		while (1)
			;
	}
}