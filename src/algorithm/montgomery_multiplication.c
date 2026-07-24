#include "algorithm.h"

BigInt *montgomery_from(BigInt *x, t_mont_ctx *ctx)
{
	BigInt *tmp = big_int_mod(x, &ctx->n);
	if (!tmp)
		return 0;

	BigInt *r = montgomery_mul(tmp, &ctx->one, &ctx->n, ctx->n0_inv);
	free(tmp);
	return r;
}

BigInt	*montgomery_to(BigInt *x, t_mont_ctx *ctx)
{
	BigInt *tmp = big_int_mod(x, &ctx->n);
	if (!tmp)
		return 0;

	BigInt *r = montgomery_mul(tmp, &ctx->r2, &ctx->n, ctx->n0_inv);
	free(tmp);
	return r;
}

static uint32_t	mont_inv32(uint32_t n)
{
	uint32_t x = 1;

	// inverse modulo 2^32 
	for (int i = 0; i < 5; i++)
		x *= 2 - n * x;

	return (uint32_t)(0 - x);
}

static BigInt	*mont_compute_r2(BigInt *n)
{
	BigInt r = {0};
	r.sign = 1;
	r.length = 1;
	r.digits[0] = 1;

	size_t count = n->length * 64;
	while (count--)
	{
		big_int_shl1(&r);
		if (big_int_cmp_abs(&r, n) >= 0)
		{
			BigInt *tmp = big_int_sub(&r, n);
			if (!tmp)
				return 0;
			r = *tmp;
			free(tmp);
		}
	}
	return big_int_copy(&r);
}

int	montgomery_ctx_init(t_mont_ctx *ctx, BigInt *n)
{
	if ((n->digits[0] & 1) == 0)
		return 0;

	// n
	ft_memcpy(&ctx->n, n, sizeof(BigInt));
	
	// R2
	BigInt *r2 = mont_compute_r2(n);
	if (!r2)
		return 0;
	ft_memcpy(&ctx->r2, r2, sizeof(BigInt));
	free(r2);

	// one
	ctx->one = (BigInt){.sign=1, .length=1, .digits={1}};

	// n0 inv
	ctx->n0_inv = mont_inv32(n->digits[0]);

	return 1;
}

BigInt *montgomery_mul(BigInt *a, BigInt *b, BigInt *n, uint32_t n0_inv)
{
	size_t k = n->length;

	uint32_t t[MAX_BIGINT_LIMB * 2 + 3];
	ft_bzero(t, sizeof(t));

	// T = a*b
	for (size_t i = 0; i < a->length; i++)
	{
		uint64_t carry = 0;
		for (size_t j = 0; j < b->length; j++)
		{
			uint64_t uv = (uint64_t)t[i+j] + (uint64_t)a->digits[i] * b->digits[j] + carry;
			t[i+j] = (uint32_t)uv;
			carry = uv >> 32;
		}

		size_t pos = i + b->length;
		while (carry)
		{
			uint64_t uv = (uint64_t)t[pos] + carry;
			t[pos] = (uint32_t)uv;
			carry = uv >> 32;
			pos++;
		}
	}

	// montgomery reduction:
	// m = t[i] * (-n^-1 % 2^32)
	for (size_t i = 0; i < k; i++)
	{
		uint32_t m = (uint32_t)((uint64_t)t[i] * n0_inv);
		uint64_t carry = 0;
		for (size_t j = 0; j < k; j++)
		{
			uint64_t uv = (uint64_t)t[i+j] + (uint64_t)m * n->digits[j] + carry;
			t[i+j] = (uint32_t)uv;
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

	// r = T / R
	BigInt *r = ft_calloc(1, sizeof(BigInt));
	if (!r)
		return 0;

	r->sign = 1;
	r->length = k + 1;
	for (size_t i = 0; i <= k; i++)
		r->digits[i] = t[i + k];

	big_int_normalize(r);

	if (big_int_cmp_abs(r, n) >= 0)
	{
		BigInt *tmp = big_int_sub(r, n);
		if (!tmp)
		{
			free(r);
			return 0;
		}
		free(r);
		r = tmp;
	}
	r->sign = 1;
	return r;
}