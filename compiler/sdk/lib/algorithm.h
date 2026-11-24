#ifndef ALGORITHM_H
#define ALGORITHM_H

#ifdef __cplusplus
# include "types/big_int.hpp"
#else
	# include "types.h"
	# include "memory.h"
	# include "stdlib.h"

	#ifndef MAX_BIGINT_LIMB
	#define MAX_BIGINT_LIMB 128
	#endif

	typedef struct BigInt
	{
		int sign;
		uint32_t digits[MAX_BIGINT_LIMB];
		size_t length;
	} BigInt;
#endif

struct s_big_int_div
{
	BigInt *quotient;
	BigInt *remainder;
};
typedef struct s_big_int_div BigIntDiv;

#ifndef kfree
# define kfree free
#endif

typedef struct s_extended_gcd
{
	BigInt *gcd;
	BigInt *x;
	BigInt *y;
} ExtendedGCD;

typedef struct s_mont_ctx
{
	BigInt n;
	BigInt r2;
	BigInt one;
	uint32_t n0_inv;
} t_mont_ctx;

#ifdef __cplusplus
extern "C" {
#endif
void	big_int_display(BigInt *n, int decimal);
BigInt *from_uint(uint32_t n);
BigInt *from_bytes(const uint8_t *bytes, size_t len);
BigInt *copy(BigInt *n);
BigInt *add(BigInt *a, BigInt *b);
BigInt *sub(BigInt *a, BigInt *b);
BigInt *mul(BigInt *a, BigInt *b);
BigIntDiv *div(BigInt *numerator, BigInt *denominator);
BigInt *gcd(BigInt *a, BigInt *b);
BigInt *mod_inverse(BigInt *e, BigInt *phi);
BigInt *mod(BigInt *numerator, BigInt *denominator);
BigInt *modular_pow(BigInt *base, BigInt *exp, BigInt *modulus);
BigInt *rng(BigInt *min, BigInt *max);
long cmp(BigInt *a, BigInt *b);
long cmp_abs(BigInt *a, BigInt *b);
void normalize(BigInt *n);
void shl1(BigInt *n);
void shr1(BigInt *n);
int is_even(BigInt *n);
int is_odd(BigInt *n);
int is_zero(BigInt *n);
size_t bit_length(BigInt *n);
uint32_t mod_small(BigInt *n, uint32_t p);
uint8_t *to_fixed_bytes(BigInt *n, size_t len);
uint8_t *get_bytes(BigInt *n, size_t *len);
ExtendedGCD *big_int_extended_gcd(BigInt *a, BigInt *b);
int miller_rabin(BigInt *n);
int montgomery_ctx_init(t_mont_ctx *ctx, BigInt *n);
BigInt *montgomery_mul(BigInt *a, BigInt *b, BigInt *n, uint32_t n0_inv);
BigInt *montgomery_from(BigInt *x, t_mont_ctx *ctx);
BigInt *montgomery_to(BigInt *x, t_mont_ctx *ctx);
#ifdef __cplusplus
}
#endif

#define big_int_from_uint from_uint
#define big_int_from_bytes from_bytes
#define big_int_copy copy
#define big_int_add add
#define big_int_sub sub
#define big_int_mul mul
#define big_int_div div
#define big_int_gcd gcd
#define big_int_mod_inverse mod_inverse
#define big_int_mod mod
#define big_int_modular_pow modular_pow
#define big_int_rng rng
#define big_int_cmp cmp
#define big_int_cmp_abs cmp_abs
#define big_int_normalize normalize
#define big_int_shl1 shl1
#define big_int_shr1 shr1
#define big_int_is_even is_even
#define big_int_is_odd is_odd
#define big_int_is_zero is_zero
#define big_int_bit_length bit_length
#define big_int_mod_small mod_small
#define big_int_to_fixed_bytes to_fixed_bytes
#define big_int_get_bytes get_bytes
#define extended_gcd big_int_extended_gcd

#endif
