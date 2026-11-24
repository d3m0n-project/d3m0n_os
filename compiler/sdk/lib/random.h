#ifndef SDK_RANDOM_H
#define SDK_RANDOM_H

#include "types.h"
#include "sys.h"

void random_init(void);
void rng_add_entropy(uint64_t value);
static inline void random_bytes(void *out, size_t len)
{
	uint8_t *bytes = (uint8_t *)out;
	while (len)
	{
		uint32_t value = random_u32();
		for (size_t i = 0; i < sizeof(value) && len; ++i, --len)
			*bytes++ = (uint8_t)(value >> (i * 8));
	}
}
uint64_t random_u64(void);

#endif
