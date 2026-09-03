#include "gpu.h"
#include "mailbox.h"

#define TAG_ALLOCATE_MEMORY			0x0003000C
#define TAG_LOCK_MEMORY				0x0003000D
#define TAG_UNLOCK_MEMORY			0x0003000E
#define TAG_RELEASE_MEMORY			0x0003000F

#define MEM_FLAG_DIRECT				0x00000004
#define MEM_FLAG_COHERENT			0x00000008
#define MEM_FLAG_L1_NONALLOCATING	0x00000004

static uint32_t gpu_handle;
static uint32_t gpu_bus_addr;

int gpu_alloc(uint32_t size, uint32_t align)
{
	volatile uint32_t msg[32] __attribute__((aligned(16)));
	int i = 0;

	msg[i++] = 0;			 // total size
	msg[i++] = 0;			 // request

	msg[i++] = TAG_ALLOCATE_MEMORY;
	msg[i++] = 12;			// value buffer size
	msg[i++] = 12;			// request size
	msg[i++] = size;
	msg[i++] = align;
	msg[i++] = MEM_FLAG_DIRECT;

	msg[i++] = 0; // end

	msg[0] = i * sizeof(uint32_t);
	if (!mailbox_call(msg, 8))
		return -1;

	gpu_handle = msg[5];
	return 0;
}

int	gpu_lock(uint32_t handle, uint32_t *bus_addr)
{
	volatile uint32_t msg[8] __attribute__((aligned(16)));
	msg[0] = sizeof(msg);
	msg[1] = 0;

	msg[2] = TAG_LOCK_MEMORY;
	msg[3] = 4;
	msg[4] = 0;
	msg[5] = handle;

	msg[6] = 0;

	if (!mailbox_call(msg, 8))
		return -1;

	*bus_addr = msg[5];
	return 0;
}


