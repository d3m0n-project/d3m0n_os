#include "algorithm.h"

BigInt	*montgomery_from(BigInt *x, t_mont_ctx *ctx)
{
	return montgomery_mul(x, &ctx->one, &ctx->n, ctx->n0_inv);
}

BigInt	*montgomery_to(BigInt *x, t_mont_ctx *ctx)
{
	return montgomery_mul(x, &ctx->r2, &ctx->n, ctx->n0_inv);
}

static uint32_t	mont_inv32(uint32_t n)
{
	uint32_t x = 1;

	// inverse modulo 2^32 
	for (int i = 0; i < 5; i++)
		x *= 2 - n * x;

	return (uint32_t)(0 - x);
}

int	montgomery_ctx_init(t_mont_ctx *ctx, BigInt *n)
{
	// n
	ft_memcpy(&ctx->n, n, sizeof(BigInt));
	
	// R2
	BigInt *r2 = big_int_from_uint(1);
	if (!r2)
		return 0;

	size_t bits = n->length * 64;
	for (size_t i = 0; i < bits; i++)
	{
		big_int_shl1(r2);
		if (big_int_cmp(r2, n) >= 0)
		{
			BigInt *tmp = big_int_sub(r2, n);
			free(r2);
			r2 = tmp;
		}
	}
	ft_memcpy(&ctx->r2, r2, sizeof(BigInt));
	free(r2);

	// one
	ctx->one = (BigInt){.sign=1, .length=1, .digits={1}};

	// n0 inv
	ctx->n0_inv = mont_inv32(n->digits[0]);

	return 1;
}

BigInt	*montgomery_mul(BigInt *a, BigInt *b, BigInt *n, uint32_t n0_inv)
{
	size_t		k = n->length;
	uint32_t	t[MAX_BIGINT_LIMB * 2 + 2];
	ft_bzero(t, sizeof(t));

	// T = A * B
	for (size_t i = 0; i < k; i++)
	{
		uint64_t carry = 0;
		for (size_t j = 0; j < k; j++)
		{
			uint64_t uv = (uint64_t)t[i + j] + (uint64_t)a->digits[i] * b->digits[j] + carry;
			t[i + j] = (uint32_t)uv;
			carry = uv >> 32;
		}

		size_t pos = i + k;
		while (carry)
		{
			uint64_t uv = (uint64_t)t[pos] + carry;
			t[pos] = (uint32_t)uv;
			carry = uv >> 32;
			pos++;
		}
	}

	// reduction
	for (size_t i = 0; i < k; i++)
	{
		uint32_t m = (uint32_t)((uint64_t)t[i] * n0_inv);
		uint64_t carry = 0;
		for (size_t j = 0; j < k; j++)
		{
			uint64_t uv = (uint64_t)t[i + j] + (uint64_t)m * n->digits[j] + carry;
			t[i + j] = (uint32_t)uv;
			carry = uv >> 32;
		}

		size_t pos = i + k;
		while (carry)
		{
			uint64_t uv = (uint64_t)t[pos] + carry;
			t[pos] = (uint32_t)uv;
			carry = uv >> 32;
			pos++;
		}
	}
	BigInt *r = ft_calloc(1, sizeof(BigInt));
	if (!r)
		return 0;

	r->length = k;
	r->sign = 1;
	for (size_t i = 0; i < k; i++)
		r->digits[i] = t[i + k];

	big_int_normalize(r);
	if (big_int_cmp_abs(r, n) >= 0)
	{
		BigInt *tmp = big_int_sub(r, n);
		free(r);
		r = tmp;
	}
	return r;
}