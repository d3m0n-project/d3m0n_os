#ifndef LOCALE_H
#define LOCALE_H

#include "types.h"

#define LC_CTYPE			0
#define LC_NUMERIC			1
#define LC_TIME				2
#define LC_COLLATE			3
#define LC_MONETARY			4
#define LC_MESSAGES			5
#define LC_ALL				6
#define LC_PAPER			7
#define LC_NAME				8
#define LC_ADDRESS			9
#define LC_TELEPHONE		10
#define LC_MEASUREMENT		11
#define LC_IDENTIFICATION	12

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

static const char *locale_names[] = {
	[LC_CTYPE]			= "C",
	[LC_NUMERIC]		= "C",
	[LC_TIME]			= "C",
	[LC_COLLATE]		= "C",
	[LC_MONETARY]		= "C",
	[LC_MESSAGES]		= "C",
	[LC_PAPER]			= "C",
	[LC_NAME]			= "C",
	[LC_ADDRESS]		= "C",
	[LC_TELEPHONE]		= "C",
	[LC_MEASUREMENT]	= "C",
	[LC_IDENTIFICATION] = "C",
};

static inline char	*setlocale(int category, const char *locale)
{
	static char current_locale[] = "C";
	if (locale == 0)
		return current_locale;

	if (category < LC_CTYPE || category > LC_IDENTIFICATION)
		return 0;

	if (locale[0] == '\0' || (locale[0] == 'C' && locale[1] == '\0'))
		return current_locale;

	return 0;
}

#endif