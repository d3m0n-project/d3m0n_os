#include "algorithm.h"

ExtendedGCD	*big_int_extended_gcd(BigInt *a, BigInt *b)
{
	BigInt *old_r = big_int_copy(a);
	BigInt *old_s = big_int_from_uint(1);
	BigInt *old_t = big_int_from_uint(0);

	BigInt *r = big_int_copy(b);
	BigInt *s = big_int_from_uint(0);
	BigInt *t = big_int_from_uint(1);
	while (r->sign != 0)
	{
		BigIntDiv *div = big_int_div(old_r, r);

		if (!div)
			return 0;

		BigInt *qr = big_int_mul(div->quotient, r);
		BigInt *new_r = big_int_sub(old_r, qr);
		kfree(qr);
		kfree(old_r);
		old_r = r;
		r = new_r;

		BigInt *qs = big_int_mul(div->quotient, s);
		BigInt *new_s = big_int_sub(old_s, qs);
		kfree(qs);
		kfree(old_s);
		old_s = s;
		s = new_s;

		BigInt *qt = big_int_mul(div->quotient, t);
		BigInt *new_t = big_int_sub(old_t, qt);
		kfree(qt);
		kfree(old_t);
		old_t = t;
		t = new_t;

		kfree(div->quotient);
		kfree(div->remainder);
		kfree(div);
	}

	BigInt *y;
	if (b->sign != 0)
	{
		BigInt		*xa = big_int_mul(old_s, a);
		BigInt		*tmp = big_int_sub(old_r, xa);
		BigIntDiv	*div = big_int_div(tmp, b);

		kfree(xa);
		kfree(tmp);
		if (!div)
			return 0;

		y = div->quotient;
		kfree(div->remainder);
		kfree(div);
	}
	else
		y = big_int_from_uint(0);


	ExtendedGCD *out = calloc(1, sizeof(ExtendedGCD));
	if (!out)
		return 0;


	out->gcd = old_r;
	out->x = old_s;
	out->y = y;
	kfree(r);
	kfree(s);
	kfree(t);

	return out;
}