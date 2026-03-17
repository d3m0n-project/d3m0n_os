#include "get_next_line.h"

static char	*move_buffer(char buffer[BUFFER_SIZE + 1], char **output, uint32_t i)
{
	*output = ft_strcat_realloc(*output, buffer, i + 1);
	ft_memmove(buffer, buffer + i + 1, BUFFER_SIZE - i);
	if (i == BUFFER_SIZE - 1)
		clear_buffer(buffer);
	return (*output);
}

static uint32_t	count_new_line(char buffer[BUFFER_SIZE + 1], uint32_t *new_line)
{
	uint32_t	i;

	i = 0;
	*new_line = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		*new_line = 1;
	return (i);
}

char	*get_next_line(int fd)
{
	static char		buffer[16][BUFFER_SIZE + 1];
	char			*output;
	uint32_t		i;
	uint32_t		new_line_found;

	output = 0;
	if (fd < 0 || fd > 16 || BUFFER_SIZE < 0)
		return (0);
	i = count_new_line(buffer[fd], &new_line_found);
	if (new_line_found && buffer[fd][i + 1] != '\0')
		return (move_buffer(buffer[fd], &output, i));
	else if (buffer[fd][0] != '\0')
		output = ft_strcat_realloc(0, buffer[fd], i + 1);
	clear_buffer(buffer[fd]);
	while (read(fd, buffer[fd], BUFFER_SIZE) > 0)
	{
		i = count_new_line(buffer[fd], &new_line_found);
		move_buffer(buffer[fd], &output, i);
		if (new_line_found)
			break ;
		clear_buffer(buffer[fd]);
	}
	return (output);
}