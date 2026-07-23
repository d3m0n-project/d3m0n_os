#include "algorithm.h"
#include "time.h"

// number of iterations for miller rabin algorithm
#define MILLER_RABIN_K	12

typedef struct
{
	BigInt			*n_minus_1;
	BigInt			*d;
	unsigned int	s;
}	Context;

int miller_witness(BigInt *n, BigInt *a, Context *context)
{
	static BigInt one = {.sign = 1, .length = 1, .digits = {1}};
	BigInt *n_minus_1 = context->n_minus_1;
	BigInt *d = context->d;
	unsigned int s = context->s;

	// x = a^d mod n
	BigInt *x = big_int_modular_pow(a, d, n);
	if (!x)
		return 1;

	if (big_int_cmp(x, &one) == 0 || big_int_cmp(x, n_minus_1) == 0)
	{
		free(x);
		return 0;
	}
	for (unsigned int i = 1; i < s; i++)
	{
		BigInt *sq = big_int_mul(x, x);
		if (!sq)
		{
			free(x);
			return 1;
		}

		BigInt *next = big_int_mod(sq, n);
		free(sq);
		free(x);
		if (!next)
			return 1;

		x = next;
		if (big_int_cmp(x, n_minus_1) == 0)
		{
			free(x);
			return 0;
		}

		if (big_int_cmp(x, &one) == 0)
		{
			free(x);
			return 1;
		}
	}
	free(x);
	return 1;
}

int	miller_rabin(BigInt *n)
{
	BigInt	two = {.sign=1, .length=1, .digits={2}};
	BigInt	one = {.sign=1, .length=1, .digits={1}};

	if (big_int_cmp(n, &two) == 0)
		return 1;

	if (big_int_cmp(n, &one) <= 0)
		return 0;

	if (big_int_is_even(n))
		return 0;

	BigInt	*n_minus_2 = big_int_sub(n, &two);
	if (!n_minus_2)
		return 0;
	
	// prepare context
	BigInt	*n_minus_1 = big_int_sub(n, &one);
	if (!n_minus_1)
	{
		free(n_minus_2);
		return 0;
	}
	// n-1 = 2^s * d
	BigInt *d = big_int_copy(n_minus_1);
	if (!d)
	{
		free(n_minus_1);
		free(n_minus_2);
		return 0;
	}
	unsigned int s = 0;
	while (big_int_is_even(d))
	{
		big_int_shr1(d);
		s++;
	}

	Context context = {.d=d, .s=s, .n_minus_1=n_minus_1};
	for (uint32_t i=0; i<MILLER_RABIN_K; i++)
	{
		// choose a number in the interval [2, n – 1]
		BigInt *a = big_int_rng(&two, n_minus_2);
		if (!a)
		{
			free(n_minus_1);
			free(d);
			free(n_minus_2);
			return 0;
		}
		if (miller_witness(n, a, &context))
		{
			free(n_minus_1);
			free(d);
			free(n_minus_2);
			free(a);
			return 0;
		}
		free(a);
	}
	free(n_minus_1);
	free(d);
	free(n_minus_2);
	return 1;
}