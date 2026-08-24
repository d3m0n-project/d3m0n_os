#include "stdio.hpp"
#include "stdlib.h"

extern "C" size_t	strlen(const char *str);

extern "C" {
	typedef struct {
		int fd;
	}	FILE;

	int	fputs(const char *string, FILE * stream)
	{
		size_t	len = strlen(string);
		write(stream->fd, string, len);
		return 0;
	}

	int	fputc(int character, FILE * stream)
	{
		char c = character;
		write(stream->fd, &c, 1);
		return 0;
	}
}