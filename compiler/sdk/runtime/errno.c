#include "runtime.h"

int			__errno = 0;
extern int	errno __attribute__((alias("__errno")));

const char	*strerror(int errnb)
{
	switch (errnb)
	{
		default:
			return "Unknown errno! TODO: implement it";
	}
	return 0;
}