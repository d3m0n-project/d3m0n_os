#include "runtime.h"

int	errno = 0;

const char	*strerror(int errnb)
{
	switch (errnb)
	{
		default:
			return "Unknown errno! TODO: implement it";
	}
	return 0;
}