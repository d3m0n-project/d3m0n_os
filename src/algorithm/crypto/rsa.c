#include "crypto.h"

static BigInt	*calc_phi(BigInt *p, BigInt *q, BigInt **p_minus_1, BigInt **q_minus_1)
{
	BigInt	one = {.length=1, .digits={1}, .sign=1};
	*p_minus_1 = big_int_sub(p, &one);
	*q_minus_1 = big_int_sub(q, &one);

	if (!*p_minus_1 || !*q_minus_1)
	{
		if (*p_minus_1)
			free(*p_minus_1);
		if (*q_minus_1)
			free(*q_minus_1);
		return 0;
	}
	return big_int_mul(*p_minus_1, *q_minus_1);
}

uint8_t	*rsa_decrypt(BigInt	*c, size_t *out_len, t_RSA_private_key *prv)
{
	BigInt	*m = big_int_modular_pow(c, &prv->d, &prv->n);
	if (!m)
		return 0;

	uint8_t	*bytes = big_int_get_bytes(m, out_len);
	free(m);
	return bytes;
}

BigInt	*rsa_encrypt(uint8_t	*bytes, size_t len, t_RSA_public_key *pub)
{
	BigInt	*m = big_int_from_bytes(bytes, len);
	BigInt	zero = {.digits={0}, .length=1, .sign=1};
	if (!m)
		return 0;

	if (big_int_cmp(m, &zero) < 0 || big_int_cmp(m, &pub->n) > 0)
	{
		log("RSA: message is too long to be encrypted! (0 <= m < n)\n", LOG_ERROR);
		free(m);
		return 0;
	}

	BigInt *c = big_int_modular_pow(m, &pub->e, &pub->n);
	if (!c)
	{
		free(m);
		return 0;
	}

	//uint8_t	padded_pkcs1[];

	return c;
}

//static int	rsa_generate_prime_number(size_t size)
//{
	//candidate |= 1                 // make it odd
	//candidate |= (1 << 1023)
//}

int	rsa_generate_keypair(size_t size, t_RSA_private_key *prv, t_RSA_public_key *pub)
{
	BigInt		e = {.length=1, .digits={65537}, .sign=1};
	BigInt		p = {.length=1, .digits={61}, .sign=1}; // random 1024-bit prime
	BigInt		q = {.length=1, .digits={53}, .sign=1}; // random 1024-bit prime
	
	(void)size;
	
	if (big_int_cmp(&p, &q) == 0)
	{
		log("RSA: Could not generate keypair, p=q\n", LOG_ERROR);
		return 0;
	}

	BigInt		*n = big_int_mul(&p, &q);
	BigInt		*p_minus_1;
	BigInt		*q_minus_1;
	BigInt		*phi_n = calc_phi(&p, &q, &p_minus_1, &q_minus_1);
	if (!n || !phi_n || !p_minus_1 || !q_minus_1)
	{
		if (n)
			free(n);
		if (phi_n)
			free(phi_n);
		if (p_minus_1)
			free(p_minus_1);
		if (q_minus_1)
			free(q_minus_1);
		return 0;
	}
	BigInt	*gcd = big_int_gcd(&e, phi_n);
	if (!gcd)
	{
		free(n);
		free(phi_n);
		free(p_minus_1);
		free(q_minus_1);
		return 0;
	}
	if (gcd->length != 1 || gcd->digits[0] != 1 || gcd->sign != 1)
	{
		free(n);
		free(phi_n);
		free(gcd);
		free(p_minus_1);
		free(q_minus_1);
		return 0; // TODO: retry calculation
	}
	
	BigInt	*d = big_int_mod_inverse(&e, phi_n);
	if (!d)
	{
		free(n);
		free(phi_n);
		free(gcd);
		free(p_minus_1);
		free(q_minus_1);
		return 0;
	}

	BigInt	*dp = big_int_mod(d, p_minus_1);
	BigInt	*dq = big_int_mod(d, q_minus_1);
	BigInt	*qInv = big_int_mod_inverse(&q, &p);
	if (!dp || !dq || !qInv)
	{
		if (qInv)
			free(qInv);
		if (dp)
			free(dp);
		if (dq)
			free(dq);
		free(n);
		free(phi_n);
		free(gcd);
		free(p_minus_1);
		free(q_minus_1);
		return 0;
	}

	// copy keys
	// public
	ft_memcpy(&pub->e, &e, sizeof(BigInt));
	ft_memcpy(&pub->n, n, sizeof(BigInt));

	// private
	ft_memcpy(&prv->n, n, sizeof(BigInt));
	ft_memcpy(&prv->e, &e, sizeof(BigInt));
	ft_memcpy(&prv->d, d, sizeof(BigInt));
	// CRT PARAMS
	ft_memcpy(&prv->p, &p, sizeof(BigInt));
	ft_memcpy(&prv->q, &q, sizeof(BigInt));
	ft_memcpy(&prv->dp, dp, sizeof(BigInt));
	ft_memcpy(&prv->dq, dq, sizeof(BigInt));
	ft_memcpy(&prv->qInv, qInv, sizeof(BigInt));

	free(n);
	free(phi_n);
	free(gcd);
	free(d);
	free(p_minus_1);
	free(q_minus_1);
	free(dp);
	free(dq);
	free(qInv);

	return 1;
}