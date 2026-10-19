#ifndef GPU_H
#define GPU_H

#include "types.h"

#define MEM_FLAG_CACHED                 0x0000000C
#define MEM_FLAG_DIRECT                 0x00000004
#define MEM_FLAG_COHERENT               0x00000008
#define MEM_FLAG_L1_NONALLOCATING       0x00000004
#define GPU_BASE                        0xC0000000
#define GPU_SHADER_MAX_SIZE             4096
#define GPU_OUTPUT_WORDS                16

struct GPU
{
	uint32_t output[GPU_OUTPUT_WORDS] __attribute__((aligned(16)));
	uint8_t  code[GPU_SHADER_MAX_SIZE] __attribute__((aligned(16)));
	uint32_t uniforms[1] __attribute__((aligned(16)));
	uint32_t mail[2] __attribute__((aligned(16)));
	uint32_t handle;
} __attribute__((aligned(16)));

uint32_t gpu_alloc(uint32_t size);
int      gpu_free(uint32_t handle);
uint32_t gpu_lock(uint32_t handle);
int      gpu_unlock(uint32_t handle);

int gpu_execute_code(const uint32_t code, const uint32_t r0, const uint32_t r1,
	const uint32_t r2, const uint32_t r3, const uint32_t r4, const uint32_t r5);
int gpu_qpu_execute(const uint32_t num_qpus, const uint32_t control,
	const uint32_t noflush, const uint32_t timeout);
int gpu_qpu_enable(const uint32_t enable);

void gpu_test(void);

#endif
