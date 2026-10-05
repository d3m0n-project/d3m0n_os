#ifndef MATH_H
#define MATH_H

static inline double	ldexp(double x, int exp)
{
	union
	{
		double d;
		unsigned long long u;
	}	v;

	unsigned long long exponent;
	int new_exp;

	v.d = x;

	// NaN or infinity
	exponent = (v.u >> 52) & 0x7FF;

	if (exponent == 0x7FF)
		return x;

	// zero
	if (exponent == 0)
		return x;

	new_exp = (int)exponent + exp;

	// overflow -> infinity
	if (new_exp >= 0x7FF)
	{
		v.u &= 0x8000000000000000ULL;
		v.u |= 0x7FF0000000000000ULL;
		return v.d;
	}

	// underflow
	if (new_exp <= 0)
	{
		v.u &= 0x8000000000000000ULL;
		return v.d = 0.0;
	}

	// replace exp
	v.u &= ~(0x7FFULL << 52);
	v.u |= (unsigned long long)new_exp << 52;

	return v.d;
}

static inline float	ldexpf(float x, int exp)
{
	union
	{
		float f;
		unsigned int u;
	}	v;

	unsigned int e;
	int new_exp;

	v.f = x;

	// NaN or infinity
	e = (v.u >> 23) & 0xFF;

	if (e == 0xFF)
		return x;

	// zero
	if (e == 0)
		return x;

	new_exp = (int)e + exp;

	// overflow
	if (new_exp >= 0xFF)
	{
		v.u &= 0x80000000U; // keep sign
		v.u |= 0x7F800000U; // infinity
		return v.f;
	}

	// underflow
	if (new_exp <= 0)
	{
		v.u &= 0x80000000U; // keep sign
		return v.f;		 	// simplified: returns signed zero
	}

	// replace exponent
	v.u &= 0x807FFFFFU;
	v.u |= (unsigned int)new_exp << 23;

	return v.f;
}

static inline float	powf(float x, float y)
{
	int		n;
	int		negative;
	float	result;

	if (y == 0.0f)
		return 1.0f;

	n = (int)y;

	// y is not integer
	if ((float)n != y)
		return 0.0f; // tmp

	negative = 0;
	if (n < 0)
	{
		negative = 1;
		n = -n;
	}

	result = 1.0f;
	while (n != 0)
	{
		if (n & 1)
			result *= x;

		x *= x;
		n >>= 1;
	}

	if (negative)
		result = 1.0f / result;

	return result;
}

static inline float	floorf(float x)
{
	union
	{
		float f;
		unsigned int u;
	} v;

	unsigned int sign;
	unsigned int exp;
	unsigned int mask;

	v.f = x;

	sign = v.u & 0x80000000U;
	exp = (v.u >> 23) & 0xFFU;

	// NaN or infinity
	if (exp == 0xFFU)
		return x;

	// |x| < 1
	if (exp < 127U)
	{
		if ((v.u & 0x7FFFFFFFU) == 0)
			return x;

		if (sign)
			return -1.0f;

		return 0.0f;
	}

	/*
	 * Already an integer if exponent >= 150.
	 * (150 - 127 = 23 fraction bits)
	 */
	if (exp >= 150U)
		return x;

	// number of fractional bits.
	mask = (1U << (150U - exp)) - 1U;

	if ((v.u & mask) == 0)
		return x;

	// remove fractional bits
	v.u &= ~mask;

	// negative non-integer
	if (sign)
		v.f -= 1.0f;

	return v.f;
}

static inline double	frexp(double x, int *exp)
{
	union
	{
		double d;
		unsigned long long u;
	}	v;

	unsigned long long	sign;
	unsigned long long	mantissa;
	unsigned int		e;
	int					exponent;

	v.d = x;
	sign = v.u & 0x8000000000000000ULL;
	e = (unsigned int)((v.u >> 52) & 0x7FFU);
	mantissa = v.u & 0x000FFFFFFFFFFFFFULL;

	// zero
	if (e == 0 && mantissa == 0)
	{
		*exp = 0;
		return x;
	}

	// NaN or infinity
	if (e == 0x7FFU)
	{
		*exp = 0;
		return x;
	}

	if (e != 0)
	{
		exponent = (int)e - 1022;

		v.u = sign
			| ((unsigned long long)1022 << 52)
			| mantissa;

		*exp = exponent;
		return v.d;
	}

	// subnormal number: normalize the mantissa manually.
	exponent = -1021;
	while ((mantissa & (1ULL << 52)) == 0)
	{
		mantissa <<= 1;
		exponent--;
	}

	mantissa &= 0x000FFFFFFFFFFFFFULL;

	v.u = sign | ((unsigned long long)1022 << 52) | mantissa;

	*exp = exponent;
	return v.d;
}

static inline float	frexpf(float x, int *exp)
{
	union
	{
		float f;
		unsigned int u;
	}	v;

	unsigned int	sign;
	unsigned int	mantissa;
	unsigned int	e;
	int				exponent;

	v.f = x;
	sign = v.u & 0x80000000U;
	e = (v.u >> 23) & 0xFFU;
	mantissa = v.u & 0x007FFFFFU;

	// zero
	if (e == 0 && mantissa == 0)
	{
		*exp = 0;
		return x;
	}

	// NaN or infinity
	if (e == 0xFFU)
	{
		*exp = 0;
		return x;
	}

	// normal number
	if (e != 0)
	{
		exponent = (int)e - 126;
		v.u = sign | (126U << 23) | mantissa;
		*exp = exponent;
		return v.f;
	}

	exponent = -126;
	while ((mantissa & 0x00800000U) == 0)
	{
		mantissa <<= 1;
		exponent--;
	}

	mantissa &= 0x007FFFFFU;

	v.u = sign | (126U << 23) | mantissa;
	*exp = exponent;
	return v.f;
}

static inline float	fmodf(float x, float y)
{
	union
	{
		float f;
		unsigned int u;
	}	ux, uy;

	unsigned int	mx;
	unsigned int	my;
	unsigned int	ex;
	unsigned int	ey;
	unsigned int	sign;
	int				shift;

	ux.f = x;
	uy.f = y;
	sign = ux.u & 0x80000000U;

	// NaN / infinity / zero div
	if ((ux.u & 0x7FFFFFFFU) > 0x7F800000U || (uy.u & 0x7FFFFFFFU) > 0x7F800000U)
		return x + y;

	if ((uy.u & 0x7FFFFFFFU) == 0)
		return 0.0f / 0.0f;

	if ((ux.u & 0x7FFFFFFFU) == 0)
		return x;

	ex = (ux.u >> 23) & 0xFFU;
	ey = (uy.u >> 23) & 0xFFU;

	mx = ux.u & 0x007FFFFFU;
	my = uy.u & 0x007FFFFFU;

	// normalize subnormal x
	if (ex == 0)
	{
		ex = 1;
		while ((mx & 0x00800000U) == 0)
		{
			mx <<= 1;
			ex--;
		}
	}
	else
		mx |= 0x00800000U;

	// normalize subnormal y
	if (ey == 0)
	{
		ey = 1;
		while ((my & 0x00800000U) == 0)
		{
			my <<= 1;
			ey--;
		}
	}
	else
		my |= 0x00800000U;

	// if |x| < |y|, the remainder is x
	if (ex < ey)
		goto result;

	// binary long division of mantissas
	shift = (int)ex - (int)ey;
	while (shift-- >= 0)
	{
		if (mx >= my)
			mx -= my;

		if (shift >= 0)
			mx <<= 1;
	}

	if (mx == 0)
	{
		ux.u = sign;
		return ux.f;
	}

	// normalize the remainder
	ex = ey;
	while ((mx & 0x00800000U) == 0)
	{
		mx <<= 1;
		ex--;
	}

	mx &= 0x007FFFFFU;

	// handle underflow into a subnormal result
	if (ex == 0)
	{
		ux.u = sign | mx;
		return ux.f;
	}

	ux.u = sign | (ex << 23) | mx;

result:
	return ux.f;
}

#endif