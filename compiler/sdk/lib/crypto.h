#ifndef SDK_CRYPTO_H
#define SDK_CRYPTO_H

#include "algorithm.h"
#include "log.h"
#include "memory.h"
#include "stdlib.h"
#include "sys.h"

static inline int sdk_file_exists(const char *path)
{
	int fd = open(path, O_READ);
	if (fd < 0)
		return 0;
	close(fd);
	return 1;
}

#define file_exists sdk_file_exists

typedef struct s_sha256_ctx
{
	uint8_t data[64];
	uint32_t datalen;
	uint64_t bitlen;
	uint32_t state[8];
} sha256_ctx;

typedef struct s_rsa_public_key
{
	BigInt n;
	BigInt e;
} t_RSA_public_key;

typedef struct s_rsa_private_key
{
	BigInt n, e, d, p, q, dp, dq, qInv;
} t_RSA_private_key;

void sha256_init(sha256_ctx *ctx);
void sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len);
void sha256_final(sha256_ctx *ctx, uint8_t hash[32]);
void chacha20_block(uint32_t out[16], const uint32_t key[8], uint32_t counter, const uint32_t nonce[3]);
char *base64_encode(uint8_t *data, size_t len);
uint8_t *base64_decode(char *data, size_t *out_len);
int rsa_generate_keypair(size_t size, t_RSA_private_key *private_key, t_RSA_public_key *public_key);
BigInt *rsa_encrypt(uint8_t *bytes, size_t len, t_RSA_public_key *public_key);
uint8_t *rsa_decrypt(BigInt *ciphertext, size_t *out_len, t_RSA_private_key *private_key);
int rsa_private_key_write_der(char *filename, t_RSA_private_key *key);
int rsa_private_key_read_der(char *filename, t_RSA_private_key *key, t_RSA_public_key *public_key);
int rsa_private_key_require(char *filename, t_RSA_private_key *private_key, t_RSA_public_key *public_key);
uint8_t *pkcs1_v1_5_generate_em(t_RSA_public_key *public_key, uint8_t *message, size_t len, size_t *out_len);
uint8_t *pkcs1_v1_5_decode_em(uint8_t *encoded, size_t len, size_t *out_len);

#endif
