#ifndef CRYPTO_H
#define CRYPTO_H

#include "types.h"
#include "algorithm.h"

// rsa
typedef struct
{
	BigInt	n;
	BigInt	e;
}	t_RSA_public_key;

typedef struct
{
	BigInt	n;
	BigInt	e;
	BigInt	d;
	BigInt	p;
	BigInt	q;
	BigInt	dp;
	BigInt	dq;
	BigInt	qInv;
}	t_RSA_private_key;

// sha256
typedef struct {
	uint8_t		data[64];
	uint32_t	datalen;
	uint64_t	bitlen;
	uint32_t	state[8];
}	sha256_ctx;

// sha256
void		sha256_init(sha256_ctx *ctx);
void		sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len);
void		sha256_final(sha256_ctx *ctx, uint8_t hash[32]);

// rsa
int			rsa_generate_keypair(size_t size, t_RSA_private_key *prv, t_RSA_public_key *pub);
BigInt		*rsa_encrypt(uint8_t	*bytes, size_t len, t_RSA_public_key *pub);
uint8_t		*rsa_decrypt(BigInt	*c, size_t *out_len, t_RSA_private_key *prv);

// chacha20
void   		chacha20_block(uint32_t out[16], const uint32_t key[8], uint32_t counter, const uint32_t nonce[3]);

#endif