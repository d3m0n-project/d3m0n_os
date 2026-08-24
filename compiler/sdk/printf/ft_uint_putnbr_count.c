#include "printf/printf.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int	ft_uint_putnbr_count(unsigned int nb, t_format f, int pad_len, int prec_len, t_buf *buffer)
{
	return ft_ull_putnbr_count((unsigned long long)nb, f, pad_len, prec_len, buffer);
}

int	ft_ul_putnbr_count(unsigned long nb, t_format f, int pad_len, int prec_len, t_buf *buffer)
{
	return ft_ull_putnbr_count((unsigned long long)nb, f, pad_len, prec_len, buffer);
}

#ifdef __cplusplus
}
#endif


