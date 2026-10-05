#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include "types.h"

/** @brief Represents a signed arbitrary-precision integer. */
class BigInt
{
private:
	int			sign;
	uint32_t	digits[MAX_BIGINT_LIMB];
	size_t		length;
public:
	/** @brief Writes the integer in decimal form. */
	void		display(int decimal);
	BigInt(uint32 n);
	BigInt(const uint8_t *bytes, size_t len);

	/** @brief Returns the sum of this arbitrary-precision integer and the operand. */
	BigInt		operator+(const BigInt& n2);
	/** @brief Returns the difference between this arbitrary-precision integer and the operand. */
	BigInt		operator-(const BigInt& n2);
	/** @brief Returns the product of this arbitrary-precision integer and the operand. */
	BigInt		operator*(const BigInt& b);
	/** @brief Divides this integer by the divisor and returns the quotient and remainder. */
	BigIntDiv	operator/(const BigInt& den);
	/** @brief Returns whether this integer is greater than the other integer. */
	int			operator>(const BigInt& n2);
	/** @brief Returns whether this integer is less than the other integer. */
	int			operator<(const BigInt& n2);
	/** @brief Shifts the stored digits one place toward the more significant end. */
	void		operator<<(void);
	/** @brief Shifts the stored digits one place toward the less significant end. */
	void		operator>>(void);

	/** @brief Compares the magnitudes of two integers. */
	long		cmp_abs(BigInt *a, BigInt *b);
	/** @brief Returns the greatest common divisor of this integer and another. */
	BigInt		gcd(const BigInt& b);
	/** @brief Returns the remainder after division by the divisor. */
	BigInt		mod(const BigInt& den);
	/** @brief Computes the modular inverse for the supplied exponent and modulus. */
	BigInt		mod_inverse(BigInt *e, BigInt *phi);
	
	/** @brief Returns a copy of this integer. */
	BigInt		clone(void);
	
	/** @brief Writes the integer as a fixed-length byte sequence. */
	uint8_t		to_fixed_bytes(BigInt *a, size_t len);
	uint8_t		*get_bytes(size_t *byte_len);
	
	/** @brief Removes redundant high-order digits and canonicalizes the sign. */
	void		normalize(void);
	BigInt		*modular_pow(BigInt *base, BigInt *exp, BigInt *mod);
	
	/** @brief Returns whether the integer is even. */
	int			is_even(void);
	/** @brief Returns whether the integer is odd. */
	int			is_odd(void);
	/** @brief Returns whether the integer is zero. */
	int			is_zero(void);
	
	/** @brief Returns the number of significant bits in the integer. */
	size_t		bit_length(void);
	BigInt		*rng(BigInt *min, BigInt *max);
	/** @brief Returns the remainder when divided by a 32-bit value. */
	uint32_t	mod_small(uint32_t p);
};

#endif