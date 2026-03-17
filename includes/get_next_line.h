#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include "filesystem.h"
# include "memory.h"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif

# if BUFFER_SIZE < 0
#  undef BUFFER_SIZE
#  define BUFFER_SIZE 0
# endif

char	*get_next_line(int fd);

uint32_t	ft_strlen(char *str);
char		*ft_strcat_realloc(char *s1, char *s2, uint32_t n);
void		clear_buffer(char buffer[BUFFER_SIZE + 1]);
void		*ft_memmove(void *dest, const void *src, uint32_t n);

#endif