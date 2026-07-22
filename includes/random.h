#ifndef RANDOM_H
#define RANDOM_H

#include "types.h"
#include "time.h"

// random
void		random_init(void);
void		rng_add_entropy(uint64_t value);
void		random_bytes(void *out, size_t len);
uint64_t	random_u64(void);
uint32_t	random_u32(void);

#endif