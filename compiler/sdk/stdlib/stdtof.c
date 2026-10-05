#include "stdlib.h"

float	strtof(const char *str, char **endptr)
{
	const char	*s;
	float		value;
	float		fraction;
	float		divisor;
	int			sign;
	int			exp_sign;
	int			exponent;

	s = str;
	value = 0.0f;
	fraction = 0.0f;
	divisor = 10.0f;
	sign = 1;
	exponent = 0;
	exp_sign = 1;

	// skip whitespace
	while (isspace(*s))
		s++;

	// sign
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}

	// integer part
	while (*s >= '0' && *s <= '9')
	{
		value = value * 10.0f + (*s - '0');
		s++;
	}

	// fractional part
	if (*s == '.')
	{
		s++;

		while (*s >= '0' && *s <= '9')
		{
			fraction = fraction * 10.0f + (*s - '0');
			divisor *= 10.0f;
			s++;
		}
	}

	value += fraction / divisor;

	// exponent
	if (*s == 'e' || *s == 'E')
	{
		const char *e = s;

		s++;

		if (*s == '+' || *s == '-')
		{
			if (*s == '-')
				exp_sign = -1;
			s++;
		}

		if (*s >= '0' && *s <= '9')
		{
			exponent = 0;

			while (*s >= '0' && *s <= '9')
			{
				exponent = exponent * 10 + (*s - '0');
				s++;
			}

			exponent *= exp_sign;
		}
		else
		{
			// invalid exponent: don't consume the 'e'
			s = e;
		}
	}

	// apply exponent
	if (exponent > 0)
	{
		while (exponent--)
			value *= 10.0f;
	}
	else
	{
		while (exponent++)
			value *= 0.1f;
	}

	value *= (float)sign;

	if (endptr)
		*endptr = (char *)s;

	return value;
}