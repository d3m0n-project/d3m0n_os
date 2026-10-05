#ifndef LOCALE_H
#define LOCALE_H

#include "types.h"

struct lconv
{
	const char	*decimal_point;
	const char	*thousands_sep;
	const char	*grouping;

	const char	*mon_decimal_point;
	const char	*mon_thousands_sep;
	const char	*mon_grouping;
	const char	*positive_sign;
	const char	*negative_sign;
	const char	*currency_symbol;

	char		int_frac_digits;
	char		frac_digits;
	char		p_cs_precedes;
	char		p_sep_by_space;
	char		n_cs_precedes;
	char		n_sep_by_space;
	char		p_sign_posn;
	char		n_sign_posn;

	const char	*int_curr_symbol;
	char 		int_p_cs_precedes;
	char 		int_n_cs_precedes;
	char 		int_p_sep_by_space;
	char 		int_n_sep_by_space;
	char 		int_p_sign_posn;
	char 		int_n_sign_posn;
};

static inline struct lconv *localeconv(void)
{
	static struct lconv l = {
		.decimal_point = ".",
		.thousands_sep = "",
		.grouping = "",

		.mon_decimal_point = "",
		.mon_thousands_sep = "",
		.mon_grouping = "",
		.positive_sign = "",
		.negative_sign = "",
		.currency_symbol = "",

		.int_frac_digits = CHAR_MAX,
		.frac_digits = CHAR_MAX,
		.p_cs_precedes = CHAR_MAX,
		.p_sep_by_space = CHAR_MAX,
		.n_cs_precedes = CHAR_MAX,
		.n_sep_by_space = CHAR_MAX,
		.p_sign_posn = CHAR_MAX,
		.n_sign_posn = CHAR_MAX,

		.int_curr_symbol = "",
		.int_p_cs_precedes = CHAR_MAX,
		.int_n_cs_precedes = CHAR_MAX,
		.int_p_sep_by_space = CHAR_MAX,
		.int_n_sep_by_space = CHAR_MAX,
		.int_p_sign_posn = CHAR_MAX,
		.int_n_sign_posn = CHAR_MAX
	};

	return &l;
}

#endif