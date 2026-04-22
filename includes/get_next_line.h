#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include "filesystem.h"
# include "memory.h"
# include "libft.h"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif

# if BUFFER_SIZE < 0
#  undef BUFFER_SIZE
#  define BUFFER_SIZE 0
# endif

char	*get_next_line(int fd);

char		*ft_strcat_realloc(char *s1, char *s2, uint32_t n);
void		clear_buffer(char buffer[BUFFER_SIZE + 1]);

#endif