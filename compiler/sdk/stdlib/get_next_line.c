#include "sys.h"
#include "stdlib.h"

#ifndef KERNEL_VERSION
char *get_next_line(int fd)
{
	char *line = 0;
	size_t length = 0;
	char character;
	int result;

	while ((result = read(fd, &character, 1)) > 0)
	{
		char *resized = realloc(line, length + 2);
		if (!resized)
		{
			free(line);
			return 0;
		}
		line = resized;
		line[length++] = character;
		line[length] = '\0';
		if (character == '\n')
			break;
	}
	if (result < 0 || length == 0)
	{
		free(line);
		return 0;
	}
	return line;
}
#endif
