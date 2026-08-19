#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include "types.h"

class BigInt
{
private:
	int			sign;
	uint32_t	digits[MAX_BIGINT_LIMB];
	size_t		length;
public:
	void		display(int decimal);
	BigInt(uint32 n);
	BigInt(const uint8_t *bytes, size_t len);

	BigInt		operator+(const BigInt& n2);
	BigInt		operator-(const BigInt& n2);
	BigInt		operator*(const BigInt& b);
	BigIntDiv	operator/(const BigInt& den);
	int			operator>(const BigInt& n2);
	int			operator<(const BigInt& n2);
	void		operator<<(void);
	void		operator>>(void);

	long		cmp_abs(BigInt *a, BigInt *b);
	BigInt		gcd(const BigInt& b);
	BigInt		mod(const BigInt& den);
	BigInt		mod_inverse(BigInt *e, BigInt *phi);
	
	BigInt		clone(void);
	
	uint8_t		to_fixed_bytes(BigInt *a, size_t len);
	uint8_t		*get_bytes(size_t *byte_len);
	
	void		normalize(void);
	BigInt		*modular_pow(BigInt *base, BigInt *exp, BigInt *mod);
	
	int			is_even(void);
	int			is_odd(void);
	int			is_zero(void);
	
	size_t		bit_length(void);
	BigInt		*rng(BigInt *min, BigInt *max);
	uint32_t	mod_small(uint32_t p);
};

#endif