#include "algorithm.h"
#include "log.h"

BigInt	*big_int_from_uint(uint32_t n)
{
	BigInt *out = ft_calloc(1, sizeof(BigInt));

	if (!out)
		return 0;

	out->sign = n ? 1 : 0;
	out->length = 1;
	out->digits[0] = n;
	return out;
}
static int	big_int_is_odd(BigInt *n)
{
	return n->digits[0] & 1;
}

static int	big_int_is_zero(BigInt *n)
{
	return n->length == 1 && n->digits[0] == 0;
}

void	big_int_display(BigInt *n, int decimal)
{
	if (n->sign == -1)
		log("-", 0);
	if (!decimal)
		log("0x", 0);
	for (int i = n->length - 1; i >= 0; i--)
	{
		if (decimal)
			log("%lu", 0, n->digits[i]);
		else
			log("%X", 0, n->digits[i]);
	}
	log("\n", 0);
}

uint8_t	*big_int_get_bytes(BigInt *n, size_t *byte_len)
{
	size_t full_len = n->length * sizeof(uint32_t);
	uint8_t *tmp = ft_calloc(full_len, sizeof(uint8_t));
	if (!tmp)
		return 0;

	for (size_t i = 0; i < n->length; i++)
	{
		uint32_t limb = n->digits[i];
		size_t j = full_len - (i + 1) * 4;

		tmp[j + 0] = (limb >> 24) & 0xFF;
		tmp[j + 1] = (limb >> 16) & 0xFF;
		tmp[j + 2] = (limb >>  8) & 0xFF;
		tmp[j + 3] =  limb        & 0xFF;
	}

	// skip zeros
	size_t first = 0;
	while (first < full_len - 1 && tmp[first] == 0)
		first++;

	*byte_len = full_len - first;
	uint8_t *out = ft_calloc(*byte_len, sizeof(uint8_t));
	if (!out)
	{
		free(tmp);
		return 0;
	}
	ft_memcpy(out, tmp + first, *byte_len);
	free(tmp);

	return out;
}

BigInt	*big_int_copy(BigInt *src)
{
	BigInt *out = ft_calloc(1, sizeof(BigInt));
	if (!out)
		return 0;

	out->sign = src->sign;
	out->length = src->length;
	for (size_t i = 0; i < src->length; i++)
		out->digits[i] = src->digits[i];
	return out;
}

BigInt *big_int_from_bytes(const uint8_t *bytes, size_t len)
{
	if (!bytes || len == 0)
		return big_int_from_uint(0);

	BigInt *out = ft_calloc(1, sizeof(BigInt));
	if (!out)
		return 0;

	out->sign = 1;
	out->length = (len + 3) / 4; // round up
	if (out->length > MAX_BIGINT_LIMB)
	{
		free(out);
		return 0;
	}
	for (size_t i = 0; i < len; i++)
	{
		size_t src = len - 1 - i;
		size_t limb = i / 4;
		size_t shift = (i % 4) * 8;
		out->digits[limb] |= ((uint32_t)bytes[src]) << shift;
	}
	big_int_normalize(out);
	return out;
}

static uint32_t	base_sub(uint32_t a, uint32_t b, uint32_t *borrow)
{
	uint64_t aa = a;
	uint64_t bb = (uint64_t)b + *borrow;

	if (aa >= bb)
	{
		*borrow = 0;
		return (uint32_t)(aa - bb);
	}

	*borrow = 1;
	return (uint32_t)((1ULL << 32) + aa - bb);
}

static uint32_t base_adder(uint32_t a, uint32_t b, uint32_t *carry)
{
	uint64_t sum = (uint64_t)a + b + *carry;
	*carry = sum >> 32;
	return (uint32_t)sum;
}

BigInt *big_int_add(BigInt *n1, BigInt *n2)
{
	if (n1->sign == 0)
		return big_int_copy(n2);

	if (n2->sign == 0)
		return big_int_copy(n1);

	if (n1->sign != n2->sign)
	{
		BigInt tmp = *n2;
		tmp.sign *= -1;
		return big_int_sub(n1, &tmp);
	}

	BigInt *out = ft_calloc(1, sizeof(BigInt));
	if (!out)
		return 0;

	out->sign = n1->sign;
	size_t out_len = max(n1->length, n2->length);
	uint32_t carry = 0;
	for (size_t i = 0; i < out_len; i++)
	{
		uint32_t a = (i < n1->length) ? n1->digits[i] : 0;
		uint32_t b = (i < n2->length) ? n2->digits[i] : 0;
		out->digits[i] = base_adder(a,b,&carry);
	}
	out->length = out_len;
	if (carry)
	{
		out->digits[out_len] = carry;
		out->length++;
	}
	return out;
}

BigInt	*big_int_sub(BigInt *n1, BigInt *n2)
{
	BigInt *out;

	// n1 - (-n2) => n1 + n2
	if (n2->sign < 0)
	{
		BigInt tmp = *n2;
		tmp.sign = 1;
		return big_int_add(n1, &tmp);
	}

	// (-n1) - n2 => -(abs(n1)+abs(n2))
	if (n1->sign < 0)
	{
		BigInt a = *n1;
		a.sign = 1;

		out = big_int_add(&a, n2);
		if (out)
			out->sign = -1;
		return out;
	}

	// both positive
	long cmp = big_int_cmp(n1, n2);

	// result is negative
	if (cmp < 0)
	{
		out = big_int_sub(n2, n1);
		if (out)
			out->sign = -1;
		return out;
	}

	out = ft_calloc(1, sizeof(BigInt));
	if (!out)
		return 0;

	uint32_t borrow = 0;

	out->length = n1->length;
	out->sign = 1;

	for (size_t i = 0; i < n1->length; i++)
	{
		uint32_t a = n1->digits[i];
		uint32_t b = (i < n2->length) ? n2->digits[i] : 0;
		out->digits[i] = base_sub(a, b, &borrow);
	}

	while (out->length > 1 && out->digits[out->length - 1] == 0)
		out->length--;

	// normalize zero
	if (out->length == 1 && out->digits[0] == 0)
		out->sign = 0;

	return out;
}

static int	big_int_get_bit(BigInt *a, size_t bit)
{
	size_t limb = bit / 32;
	size_t offset = bit % 32;

	if (limb >= a->length)
		return 0;

	return (a->digits[limb] >> offset) & 1;
}

void	big_int_normalize(BigInt *n)
{
	while (n->length > 1 && n->digits[n->length - 1] == 0)
		n->length--;

	if (n->length == 1 && n->digits[0] == 0)
		n->sign = 0;
	else
		n->sign = 1;
}

static void big_int_shl1(BigInt *a)
{
	uint32_t carry = 0;

	for (size_t i = 0; i < a->length; i++)
	{
		uint64_t value = ((uint64_t)a->digits[i] << 1) | carry;

		a->digits[i] = (uint32_t)value;
		carry = value >> 32;
	}

	if (carry && a->length < MAX_BIGINT_LIMB)
	{
		a->digits[a->length] = carry;
		a->length++;
	}
	big_int_normalize(a);
}


static void	big_int_shr1(BigInt *a)
{
	uint32_t carry = 0;
	for (size_t i = a->length; i-- > 0;)
	{
		uint32_t newcarry = a->digits[i] & 1;
		a->digits[i] >>= 1;
		a->digits[i] |= carry << 31;
		carry = newcarry;
	}
	big_int_normalize(a);
}

static void	big_int_set_bit(BigInt *a, size_t bit)
{
	size_t limb = bit / 32;
	size_t offset = bit % 32;

	if (limb >= MAX_BIGINT_LIMB)
		return;

	a->digits[limb] |= (1u << offset);

	if (a->length <= limb)
		a->length = limb + 1;
}

long	big_int_cmp(BigInt *n1, BigInt *n2)
{
	if (n1->sign != n2->sign)
		return (n1->sign > n2->sign) ? 1 : -1;

	if (n1->sign == 0)
		return 0;

	long mag = 0;
	if (n1->length != n2->length)
		mag = (n1->length > n2->length) ? 1 : -1;
	else
	{
		for (size_t i = n1->length; i-- > 0;)
		{
			if (n1->digits[i] != n2->digits[i])
			{
				mag = (n1->digits[i] > n2->digits[i]) ? 1 : -1;
				break;
			}
		}
	}
	return n1->sign * mag;
}

static long	big_int_cmp_abs(BigInt *a, BigInt *b)
{
	if (a->length != b->length)
		return a->length > b->length ? 1 : -1;
	for (size_t i = a->length; i-- > 0;)
	{
		if (a->digits[i] != b->digits[i])
			return a->digits[i] > b->digits[i] ? 1 : -1;
	}
	return 0;
}

BigIntDiv	*big_int_div(BigInt *num, BigInt *den)
{
	if (!num || !den)
		return 0;

	if (den->length == 1 && den->digits[0] == 0)
		return 0;

	BigIntDiv *out = ft_calloc(1, sizeof(BigIntDiv));
	if (!out)
		return 0;

	out->quotient = ft_calloc(1, sizeof(BigInt));
	out->remainder = ft_calloc(1, sizeof(BigInt));
	if (!out->quotient || !out->remainder)
	{
		free(out->quotient);
		free(out->remainder);
		free(out);
		return 0;
	}


	out->quotient->length = 1;
	out->remainder->length = 1;
	if (big_int_cmp_abs(num, den) < 0)
	{
		free(out->remainder);
		out->remainder = big_int_copy(num);
		out->quotient->sign = 0;
		big_int_normalize(out->remainder);
		return out;
	}

	// find highest bit
	size_t bits = num->length * 32;
	while (bits > 0 && big_int_get_bit(num, bits - 1) == 0)
		bits--;

	for (size_t i = bits; i-- > 0;)
	{
		big_int_shl1(out->remainder);
		if (big_int_get_bit(num, i))
		{
			out->remainder->digits[0] |= 1;
			out->remainder->sign = 1;
		}

		if (big_int_cmp_abs(out->remainder, den) >= 0)
		{
			BigInt *tmp = big_int_sub(out->remainder, den);
			if (!tmp)
			{
				free(out->quotient);
				free(out->remainder);
				free(out);
				return 0;
			}
			free(out->remainder);
			out->remainder = tmp;
			big_int_normalize(out->remainder);
			// quotient bit = 1
			big_int_set_bit(out->quotient, i);
		}
	}
	big_int_normalize(out->quotient);
	big_int_normalize(out->remainder);

	out->quotient->sign = out->quotient->sign == 0 ? 0 : num->sign * den->sign;
	out->remainder->sign = out->remainder->sign == 0 ? 0 : num->sign;
	return out;
}


BigInt	*big_int_mul(BigInt *a, BigInt *b)
{
	BigInt *out = ft_calloc(1, sizeof(BigInt));
	if (!out)
		return 0;

	out->sign = a->sign * b->sign;

	size_t max = a->length + b->length;
	if (max > MAX_BIGINT_LIMB)
	{
		free(out);
		return 0;
	}

	for (size_t i = 0; i<a->length; i++)
	{
		uint64_t carry = 0;

		for (size_t j = 0; j<b->length; j++)
		{
			uint64_t sum = (uint64_t)a->digits[i] * (uint64_t)b->digits[j] + out->digits[i + j] + carry;
			out->digits[i + j] = (uint32_t)sum;
			carry = sum >> 32;
		}
		out->digits[i + b->length] += (uint32_t)carry;
	}

	out->length = max;
	while (out->length > 1 && out->digits[out->length - 1] == 0)
		out->length--;
	if (out->length == 1 && out->digits[0] == 0)
		out->sign = 0;
	else
		out->sign = a->sign * b->sign;
	return out;
}

BigInt *big_int_gcd(BigInt *a, BigInt *b)
{
	BigInt *x = big_int_copy(a);
	BigInt *y = big_int_copy(b);

	if (!x || !y)
	{
		free(x);
		free(y);
		return 0;
	}

	x->sign = 1;
	y->sign = 1;
	while (!(y->length == 1 && y->digits[0] == 0))
	{
		BigIntDiv *div = big_int_div(x, y);
		if (!div)
		{
			free(x);
			free(y);
			return 0;
		}

		free(x);
		x = y;
		y = div->remainder;
		free(div->quotient);
		free(div);
	}

	free(y);
	x->sign = 1;
	return x;
}

BigInt	*big_int_mod_inverse(BigInt *e, BigInt *phi)
{
	ExtendedGCD *res = big_int_extended_gcd(e, phi);
	if (!res)
		return 0;

	// e and phi must be coprime
	if (res->gcd->length != 1 || res->gcd->digits[0] != 1)
	{
		free(res->gcd);
		free(res->x);
		free(res->y);
		free(res);
		return 0;
	}


	// d = x mod phi
	BigInt *d;
	if (res->x->sign < 0)
	{
		// negative x: d = phi - |x|
		BigInt abs_x = *res->x;
		abs_x.sign = 1;
		d = big_int_sub(phi, &abs_x);
	}
	else
		d = big_int_copy(res->x);


	// cleanup
	free(res->gcd);
	free(res->x);
	free(res->y);
	free(res);
	return d;
}

BigInt	*big_int_mod(BigInt *div, BigInt *den)
{
	BigIntDiv	*division_output = big_int_div(div, den);
	if (!division_output)
		return 0;
	
	BigInt	*output = big_int_copy(division_output->remainder);
	free(division_output->remainder);
	free(division_output->quotient);
	free(division_output);
	return output;
}

BigInt	*big_int_modular_pow(BigInt *base, BigInt *exp, BigInt *mod)
{
	BigInt	*result = big_int_from_uint(1);
	BigInt	*b = big_int_mod(base, mod);
	BigInt	*e = big_int_copy(exp);

	if (!result || !b || !e)
		goto error;

	while (!big_int_is_zero(e))
	{
		if (big_int_is_odd(e))
		{
			BigInt *tmp = big_int_mul(result, b);
			BigInt *r = big_int_mod(tmp, mod);

			free(tmp);
			free(result);
			result = r;
			if (!result)
				goto error;
		}

		BigInt *tmp = big_int_mul(b, b);
		BigInt *bb = big_int_mod(tmp, mod);
		free(tmp);
		free(b);
		b = bb;
		if (!b)
			goto error;
		big_int_shr1(e);
	}
	free(b);
	free(e);
	return result;

error:
	free(result);
	free(b);
	free(e);
	return 0;
}