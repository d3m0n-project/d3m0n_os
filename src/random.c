#include "random.h"
#include "time.h"
#include "crypto.h"

struct rng_state {
	uint32_t	key[8];	  // 256-bit ChaCha key
	uint32_t	nonce[3];	// 96-bit nonce
	uint32_t	counter;
	int			initialized;
};

static struct rng_state	rng;
static uint64_t entropy_pool = 0;

void random_init(void)
{
	uint8_t		hash[32];
	sha256_ctx	c;

	sha256_init(&c);
	sha256_update(&c, (uint8_t *)&entropy_pool, sizeof(entropy_pool));
	sha256_final(&c, hash);

	for(int i = 0; i < 8; i++)
		rng.key[i] = hash[i*4] | hash[i*4+1]<<8 | hash[i*4+2]<<16 | hash[i*4+3]<<24;

	rng.nonce[0] = time_us();
	rng.nonce[1] = entropy_pool;
	rng.nonce[2] = entropy_pool >> 32;

	rng.counter = 0;
	rng.initialized = 1;
}


void	rng_add_entropy(uint64_t value)
{
	entropy_pool ^= value;
	entropy_pool *= 0x9E3779B97F4A7C15ULL;
	entropy_pool ^= entropy_pool >> 32;
}

void random_bytes(void *out, size_t len)
{
	uint8_t *dst = (uint8_t *)out;

	if (!rng.initialized)
		random_init();

	while (len > 0)
	{
		uint32_t block[16];

		chacha20_block(block, rng.key, rng.counter++, rng.nonce);
		size_t copy = len;

		if (copy > 64)
			copy = 64;

		uint8_t *src = (uint8_t *)block;
		for (size_t i = 0; i < copy; i++)
			dst[i] = src[i];

		dst += copy;
		len -= copy;
	}
}

uint64_t	random_u64(void)
{
	uint64_t n;

	random_bytes(&n, sizeof(n));
	return n;
}

uint32_t random_u32(void)
{
	uint32_t n;

	random_bytes(&n, sizeof(n));
	return n;
}