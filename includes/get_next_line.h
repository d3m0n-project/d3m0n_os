#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include "filesystem.h"
# include "memory.h"
# include "libft.h"

#define TMP_BUFFER_SIZE	4096
#define BUFFER_SIZE		(((TMP_BUFFER_SIZE + 1U) + 3U) & ~3U)

char		*get_next_line(int fd);
void		clear_fd_buffer(int fd);

char		*ft_strcat_realloc(char *s1, char *s2, uint32_t n);
void		clear_buffer(char buffer[BUFFER_SIZE + 1]);

#endif