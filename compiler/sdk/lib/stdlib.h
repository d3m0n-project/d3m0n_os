#ifndef STDLIB_H
#define STDLIB_H

#include "types.h"
#include "memory.h"
#include "sys.h"


#ifdef __cplusplus
extern "C" {
#endif
	/** @brief Represents one node in a singly linked list. */
	typedef struct s_list
	{
		void			*content;
		struct s_list	*next;
	}	t_list;

	#define max(a, b)		((a > b)?a:b)
	#define min(a, b)		((a < b)?a:b)

	
	int				islower(int c);
	int				isupper(int c);
	int				isgraph(int c);
	int				iscntrl(int c);
	int				ispunct(int c);
	int				isalpha(int c);
	int				isdigit(int c);
	int				isxdigit(int c);
	int				isalnum(int c);
	int				isascii(int c);
	int				isprint(int c);
	size_t			strlen(const char *str);
	void			*memset(void *str, int c, size_t len);
	void			bzero(void *str, size_t len);
	void			*memcpy(void *dest, const void *src, size_t len);
	void			*memmove(void *dest, const void *src, size_t len);
	size_t  		strlcpy(char *dst, const char *src, size_t size);
	size_t  		strlcat(char *dst, const char *src, size_t size);
	size_t			strcpy(char *dst, const char *src);
	size_t  		strcspn(const char *s, const char *reject);
	int				tolower(int c);
	int				toupper(int c);
	char			*strchr(const char *str, int c);
	char			*strpbrk(const char *str, const char *brk);
	char			*strrchr(const char *str, int c);
	char			*strnstr(const char *into, const char *substring, size_t len);
	char			*strstr(const char *haystack, const char *needle);
	size_t			strspn(const char *s, const char *accept);
	int				strncmp(const char *s1, const char *s2, size_t len);
	int				strcmp(const char *s1, const char *s2);
	void			*memchr(const void *str, int c, size_t len);
	int				memcmp(const void *s1, const void *s2, size_t len);
	int				atoi(char *str);
	float			strtof(const char *str, char **endptr);
	unsigned long	strtoul(const char *nptr, char **endptr, int base);
	char			*strncat(char *dst, const char *src, size_t n);
	char			*strcat(char *dst, const char *src);
	int				strcoll(const char *s1, const char *s2);

	char			*strdup(const char *src);

	int				isspace(char c);
	char			*substr(char const *str, unsigned int start, size_t len);
	char			*strjoin(char const *s1, char const *s2);
	char			*strtrim(char const *s1, char const *set);
	char			**split(char const *str, char c);
	char			*itoa(int n);
	char			*strmapi(char const *str, char (*f)(unsigned int, char));
	void			striteri(char *str, void (*f)(unsigned int, char*));
	void			putchar_fd(char c, int fd);
	void			putstr_fd(char *str, int fd);
	void			putendl_fd(char *str, int fd);
	void			putnbr_fd(int n, int fd);

	t_list			*lstnew(void *content);
	void			lstadd_front(t_list **lst, t_list *n);
	int				lstsize(t_list *lst);
	t_list			*lstlast(t_list *lst);
	void			lstadd_back(t_list **lst, t_list *n);
	void			lstdelone(t_list *lst, void (*del)(void*));
	void			lstclear(t_list **lst, void (*del)(void*));
	void			lstiter(t_list *lst, void (*f)(void *));
	t_list			*lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));

	void			cleanup_splitted(char **splitted);


	// system commands
	static inline int	system(const char *command)
	{
		write(1, "TODO: system()\n", 16);
		(void)command;
		return 0;
	}

#ifdef __cplusplus
}
#endif

#endif