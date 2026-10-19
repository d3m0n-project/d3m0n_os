#include "display/gpu.h"
#include "log.h"
#include "memory/mmu.h"
#include "protocol/mailbox.h"
#include "time.h"

#define TAG_ALLOCATE_MEMORY  0x0003000C
#define TAG_LOCK_MEMORY      0x0003000D
#define TAG_UNLOCK_MEMORY    0x0003000E
#define TAG_RELEASE_MEMORY   0x0003000F
#define TAG_EXECUTE_CODE     0x00030010
#define TAG_EXECUTE_QPU      0x00030011
#define TAG_ENABLE_QPU       0x00030012
#define GPU_MEM_ALIGNMENT    4096

#define V3D_BASE             (PERIPHERAL_BASE + 0x00C00000U)
#define V3D_L2CACTL          (0x00020U / 4)
#define V3D_SLCACTL          (0x00024U / 4)
#define V3D_SRQPC            (0x00430U / 4)
#define V3D_SRQUA            (0x00434U / 4)
#define V3D_SRQCS            (0x0043CU / 4)
#define V3D_DBCFG            (0x00E00U / 4)
#define V3D_DBQITE           (0x00E2CU / 4)
#define V3D_DBQITC           (0x00E30U / 4)

static int gpu_tag_responded(volatile uint32_t *msg, uint32_t min_size)
{
	return (msg[4] & 0x80000000U) != 0 && (msg[4] & 0x7FFFFFFFU) >= min_size;
}

uint32_t gpu_alloc(uint32_t size)
{
	volatile uint32_t msg[9] __attribute__((aligned(16))) = {0};

	msg[0] = sizeof(msg);
	msg[2] = TAG_ALLOCATE_MEMORY;
	msg[3] = 12;
	msg[4] = 12;
	msg[5] = size;
	msg[6] = GPU_MEM_ALIGNMENT;
	msg[7] = MEM_FLAG_DIRECT;
	if (!mailbox_call(msg, MAILBOX_CH_PROP))
	{
		log("GPU alloc mailbox error: status=%x tag=%x response=%x handle=%x\n", LOG_ERROR, msg[1], msg[2], msg[4], msg[5]);
		return 0;
	}
	if (!gpu_tag_responded(msg, sizeof(uint32_t)) || msg[5] == 0)
	{
		log("GPU alloc returned no handle: status=%x response=%x size=%u align=%u flags=%x value=%x\n", LOG_ERROR, msg[1], msg[4], size, GPU_MEM_ALIGNMENT, MEM_FLAG_DIRECT, msg[5]);
		return 0;
	}
	return msg[5];
}

int gpu_free(uint32_t handle)
{
	volatile uint32_t msg[7] __attribute__((aligned(16))) = {0};

	msg[0] = sizeof(msg);
	msg[2] = TAG_RELEASE_MEMORY;
	msg[3] = 4;
	msg[4] = 4;
	msg[5] = handle;
	if (!mailbox_call(msg, MAILBOX_CH_PROP) || !gpu_tag_responded(msg, sizeof(uint32_t)) || msg[5] != 0)
		return -1;
	return 0;
}

uint32_t gpu_lock(uint32_t handle)
{
	volatile uint32_t msg[7] __attribute__((aligned(16))) = {0};
	int mailbox_ok;

	msg[0] = sizeof(msg);
	msg[2] = TAG_LOCK_MEMORY;
	msg[3] = 4;
	msg[4] = 4;
	msg[5] = handle;
	mailbox_ok = mailbox_call(msg, MAILBOX_CH_PROP);
	if (!mailbox_ok || !gpu_tag_responded(msg, sizeof(uint32_t)) || (msg[5] & 0xC0000000U) != GPU_BASE)
	{
		log("GPU lock failed: mailbox=%x tag=%x size=%u response=%x value=%x\n", LOG_ERROR, msg[1], msg[2], msg[3], msg[4], msg[5]);
		return 0;
	}
	return msg[5];
}

int gpu_unlock(uint32_t handle)
{
	volatile uint32_t msg[7] __attribute__((aligned(16))) = {0};

	msg[0] = sizeof(msg);
	msg[2] = TAG_UNLOCK_MEMORY;
	msg[3] = 4;
	msg[4] = 4;
	msg[5] = handle;
	if (!mailbox_call(msg, MAILBOX_CH_PROP) || !gpu_tag_responded(msg, sizeof(uint32_t)) || msg[5] != 0)
		return -1;
	return 0;
}

int gpu_execute_code(const uint32_t code, const uint32_t r0, const uint32_t r1,
	const uint32_t r2, const uint32_t r3, const uint32_t r4, const uint32_t r5)
{
	volatile uint32_t msg[13] __attribute__((aligned(16))) = {0};

	msg[0] = sizeof(msg);
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
	if (!mailbox_call(msg, MAILBOX_CH_PROP) || !gpu_tag_responded(msg, sizeof(uint32_t)))
		return -1;
	return (int)msg[5];
}

int	gpu_qpu_execute(uint32_t num_qpus, uint32_t control, uint32_t noflush, uint32_t timeout)
{
	volatile uint32_t *v3d = (volatile uint32_t *)(uintptr_t)V3D_BASE;
	volatile uint32_t *mail;
	uint64_t start;
	uint32_t q;

	if (num_qpus == 0 || num_qpus > 16 || control == 0 || (control & 0xC0000000U) != GPU_BASE)
		return -1;
	
	mail = (volatile uint32_t *)(uintptr_t)(control & 0x3FFFFFFFU);
	if (mail[0] == 0 || mail[1] == 0)
		return -1;
	
	if (!noflush)
	{
		v3d[V3D_DBCFG] = 0;
		v3d[V3D_DBQITE] = 0;
		v3d[V3D_DBQITC] = 0xFFFFFFFFU;
		v3d[V3D_L2CACTL] = 1U << 2;
		v3d[V3D_SLCACTL] = 0xFFFFFFFFU;
	}

	v3d[V3D_SRQCS] = (1U << 7) | (1U << 8) | (1U << 16);
	mmu_barrier();
	for (q = 0; q < num_qpus; ++q)
	{
		v3d[V3D_SRQUA] = mail[1];
		v3d[V3D_SRQPC] = mail[0];
	}
	mmu_barrier();
	start = time_us();
	while (((v3d[V3D_SRQCS] >> 16) & 0xFFU) != num_qpus)
	{
		if (timeout != 0 && time_us() - start >= (uint64_t)timeout * 1000ULL)
			return -1;
	}
	return 0;
}

int	gpu_qpu_enable(uint32_t enable)
{
	volatile uint32_t msg[7] __attribute__((aligned(16))) = {0};

	msg[0] = sizeof(msg);
	msg[2] = TAG_ENABLE_QPU;
	msg[3] = 4;
	msg[4] = 4;
	msg[5] = enable ? 1 : 0;
	if (!mailbox_call(msg, MAILBOX_CH_PROP))
	{
		log("QPU enable mailbox error: status=%x response=%x value=%x\n", LOG_ERROR, msg[1], msg[4], msg[5]);
		return -1;
	}
	if (!gpu_tag_responded(msg, sizeof(uint32_t)) || msg[5] != 0)
	{
		log("QPU enable rejected: status=%x response=%x value=%x requested=%u\n", LOG_ERROR, msg[1], msg[4], msg[5], enable ? 1U : 0U);
		return -1;
	}
	return 0;
}
