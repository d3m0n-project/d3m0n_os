#include "display/gpu.h"
#include "protocol/mailbox.h"

#define TAG_ALLOCATE_MEMORY			0x0003000C
#define TAG_LOCK_MEMORY				0x0003000D
#define TAG_UNLOCK_MEMORY			0x0003000E
#define TAG_RELEASE_MEMORY			0x0003000F

#define MEM_FLAG_DIRECT				0x00000004
#define MEM_FLAG_COHERENT			0x00000008
#define MEM_FLAG_L1_NONALLOCATING	0x00000004

#define TAG_UNLOCK_MEMORY			0x0003000E
#define TAG_EXECUTE_CODE			0x00030010
#define TAG_EXECUTE_QPU				0x00030011
#define TAG_ENABLE_QPU				0x00030012

static uint32_t		gpu_handle;

int	gpu_alloc(uint32_t size, uint32_t align)
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


int	gpu_unlock(void)
{
	volatile uint32_t msg[7] __attribute__((aligned(16)));

	msg[0] = sizeof(msg);
	msg[1] = 0;
	msg[2] = TAG_UNLOCK_MEMORY;
	msg[3] = 4;
	msg[4] = 4;
	msg[5] = gpu_handle;
	msg[6] = 0;

	if (!mailbox_call(msg, 8) || msg[1] != 0x80000000 || msg[4] != 0x80000004 || msg[5] != 0)
		return -1;

	return 0;
}

int	gpu_execute_code(const uint32_t code, const uint32_t r0, const uint32_t r1, const uint32_t r2, const uint32_t r3, const uint32_t r4, const uint32_t r5)
{
	volatile uint32_t msg[13] __attribute__((aligned(16)));

	msg[0] = sizeof(msg);
	msg[1] = 0;
	msg[2] = TAG_EXECUTE_CODE;
	msg[3] = 28;
	msg[4] = 28;
	msg[5] = code;
	msg[6] = r0;
	msg[7] = r1;
	msg[8] = r2;
	msg[9] = r3;
	msg[10] = r4;
	msg[11] = r5;
	msg[12] = 0;

	if (!mailbox_call(msg, 8) || msg[1] != 0x80000000 || msg[4] != 0x80000004)
		return -1;

	return (int)msg[5];
}

int gpu_qpu_execute(const uint32_t num_qpus, const uint32_t control, const uint32_t noflush, const uint32_t timeout)
{
	volatile uint32_t msg[10] __attribute__((aligned(16)));

	msg[0] = sizeof(msg);
	msg[1] = 0;
	msg[2] = TAG_EXECUTE_QPU;
	msg[3] = 16;
	msg[4] = 16;
	msg[5] = num_qpus;
	msg[6] = control;
	msg[7] = noflush;
	msg[8] = timeout;
	msg[9] = 0;

	if (!mailbox_call(msg, 8) || msg[1] != 0x80000000 || msg[4] != 0x80000010)
		return -1;

	return (int)msg[5];
}

int	gpu_qpu_enable(const uint32_t enable)
{
	volatile uint32_t msg[7] __attribute__((aligned(16)));

	msg[0] = sizeof(msg);
	msg[1] = 0;
	msg[2] = TAG_ENABLE_QPU;
	msg[3] = 4;
	msg[4] = 4;
	msg[5] = enable ? 1 : 0;
	msg[6] = 0;

	if (!mailbox_call(msg, 8) || msg[1] != 0x80000000 || msg[4] != 0x80000004)
		return -1;

	return 0;
}