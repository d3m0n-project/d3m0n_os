#ifndef PRINTF_H
#define PRINTF_H

#include "types.h"
#include "sys.h"
#include "stdlib.h"

#define putc(c) do { \
    char _c = (c);    \
    write(1, &_c, 1); \
} while (0)

#define FLAG_MINUS 0
#define FLAG_ZERO 1
#define FLAG_HASHTAG 2
#define FLAG_SPACE 3
#define FLAG_PLUS 4
#define WIDTH_UNRESTRICTED -1

#ifdef __cplusplus
extern "C" {
#endif
typedef enum
{
		LEN_NONE,
		LEN_L,
		LEN_LL
}		e_length;

typedef struct s_format
{
		char			flags[6];
		int				width;
		int				precision;
		char			type;
		e_length		length;
}		t_format;

t_format		parse_format(char *str, size_t *i);

int				ft_putnbr_count(int nb, t_format f, int pad_len, int prec_len);
int				ft_putstr_count(char *str, t_format f);
int				ft_putchar_count(char c, t_format f);
int				ft_uint_putnbr_count(uint32_t nb, t_format f, int pad, int prec);
int				ft_ul_putnbr_count(unsigned long nb, t_format f, int pad_len, int prec_len);
int				ft_ull_putnbr_count(unsigned long long nb, t_format f, int pad_len, int prec_len);
int				ft_putptr_count(void *ptr, t_format f);
int				ft_puthex_count(uint32_t i, t_format f, int pad_len, int prec_len);

int				display_hex(size_t nb, int in_lowercase);
int				display_uint(unsigned int nb);
int				display_ull(unsigned long long nb);

int				ft_put_precision_rep(int already_printed);
int				ft_put_padding_rep(int count, char pad);

#ifdef __cplusplus
}
#endif

#endif