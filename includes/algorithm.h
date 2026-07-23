#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "types.h"
#include "math.h"
#include "libft.h"
#include "memory.h"

#define MAX_BIGINT_LIMB	128

typedef struct {
	int			sign;
	uint32_t	digits[MAX_BIGINT_LIMB];
	size_t		length;
}	BigInt;

typedef struct
{
	BigInt *quotient;
	BigInt *remainder;
}	BigIntDiv;

typedef struct
{
	BigInt	*gcd;
	BigInt	*x;
	BigInt	*y;
}	ExtendedGCD;

typedef struct s_mont_ctx
{
	BigInt	  	n;		// modulus
	BigInt	  	r2;		// R2 mod n
	BigInt	  	one;	// 1
	uint32_t	n0_inv;
}	t_mont_ctx;

// big int functions
void		big_int_display(BigInt *n, int decimal);
BigInt		*big_int_add(BigInt *n1, BigInt *n2);
BigInt		*big_int_sub(BigInt *n1, BigInt *n2);
BigInt		*big_int_mul(BigInt *a, BigInt *b);
BigIntDiv	*big_int_div(BigInt *num, BigInt *den);
long		big_int_cmp(BigInt *n1, BigInt *n2);
long		big_int_cmp_abs(BigInt *a, BigInt *b);
BigInt		*big_int_gcd(BigInt *a, BigInt *b);
BigInt		*big_int_from_uint(uint32_t n);
BigInt		*big_int_mod_inverse(BigInt *e, BigInt *phi);
BigInt		*big_int_mod(BigInt *div, BigInt *den);
BigInt		*big_int_copy(BigInt *src);
BigInt		*big_int_from_bytes(const uint8_t *bytes, size_t len);
void		big_int_normalize(BigInt *n);
BigInt		*big_int_modular_pow(BigInt *base, BigInt *exp, BigInt *mod);
uint8_t		*big_int_get_bytes(BigInt *n, size_t *byte_len);
int			big_int_is_even(BigInt *n);
int			big_int_is_odd(BigInt *n);
int			big_int_is_zero(BigInt *n);
void		big_int_shl1(BigInt *a);
void		big_int_shr1(BigInt *a);
size_t		big_int_bit_length(BigInt *n);
BigInt		*big_int_rng(BigInt *min, BigInt *max);
uint32_t	big_int_mod_small(BigInt *n, uint32_t p);

// extended euclidian algorithm
ExtendedGCD	*big_int_extended_gcd(BigInt *a, BigInt *b);

// miller_rabin
int			miller_rabin(BigInt *n);

// montgomery
int			montgomery_ctx_init(t_mont_ctx *ctx, BigInt *n);
BigInt		*montgomery_reduce(uint64_t *t, BigInt *n);
BigInt		*montgomery_mul(BigInt *a, BigInt *b, BigInt *n, uint32_t n0_inv);
BigInt		*montgomery_to(BigInt *x, t_mont_ctx *ctx);
BigInt		*montgomery_from(BigInt *x, t_mont_ctx *ctx);


#endif