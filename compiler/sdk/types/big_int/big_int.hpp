#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include "types.h"

/** @brief Represents the BigInt type. */
class BigInt
{
private:
	int			sign;
	uint32_t	digits[MAX_BIGINT_LIMB];
	size_t		length;
public:
	/** @brief display operation. */
	void		display(int decimal);
	BigInt(uint32 n);
	BigInt(const uint8_t *bytes, size_t len);

	/** @brief Implements the + operation. */
	BigInt		operator+(const BigInt& n2);
	/** @brief Implements the - operation. */
	BigInt		operator-(const BigInt& n2);
	/** @brief Implements the * operation. */
	BigInt		operator*(const BigInt& b);
	/** @brief Implements the / operation. */
	BigIntDiv	operator/(const BigInt& den);
	/** @brief Implements the > operation. */
	int			operator>(const BigInt& n2);
	/** @brief Implements the < operation. */
	int			operator<(const BigInt& n2);
	/** @brief Implements the << operation. */
	void		operator<<(void);
	/** @brief Implements the >> operation. */
	void		operator>>(void);

	/** @brief cmp_abs operation. */
	long		cmp_abs(BigInt *a, BigInt *b);
	/** @brief gcd operation. */
	BigInt		gcd(const BigInt& b);
	/** @brief mod operation. */
	BigInt		mod(const BigInt& den);
	/** @brief mod_inverse operation. */
	BigInt		mod_inverse(BigInt *e, BigInt *phi);
	
	/** @brief clone operation. */
	BigInt		clone(void);
	
	/** @brief to_fixed_bytes operation. */
	uint8_t		to_fixed_bytes(BigInt *a, size_t len);
	uint8_t		*get_bytes(size_t *byte_len);
	
	/** @brief normalize operation. */
	void		normalize(void);
	BigInt		*modular_pow(BigInt *base, BigInt *exp, BigInt *mod);
	
	/** @brief is_even operation. */
	int			is_even(void);
	/** @brief is_odd operation. */
	int			is_odd(void);
	/** @brief is_zero operation. */
	int			is_zero(void);
	
	/** @brief bit_length operation. */
	size_t		bit_length(void);
	BigInt		*rng(BigInt *min, BigInt *max);
	/** @brief mod_small operation. */
	uint32_t	mod_small(uint32_t p);
};

#endif