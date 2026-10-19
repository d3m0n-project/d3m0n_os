#include "display/gpu.h"
#include "d3m0n.h"
#include "filesystem/fat32.h"
#include "log.h"
#include "memory/mmu.h"
#include "protocol/mailbox.h"

static uint32_t gpu_bus_address(const void *address)
{
	return GPU_BASE + (uint32_t)(uintptr_t)address;
}

static volatile struct GPU *gpu_create(void)
{
	uint32_t handle;
	uint32_t bus_address;
	volatile struct GPU *gpu;

	if (gpu_qpu_enable(1) != 0)
		panic("Failed to enable QPU\n");
	
	handle = gpu_alloc(sizeof(struct GPU));
	if (handle == 0)
	{
		gpu_qpu_enable(0);
		panic("Failed to allocate shared QPU memory\n");
	}
	bus_address = gpu_lock(handle);
	if ((bus_address & 0xC0000000U) != GPU_BASE)
	{
		gpu_free(handle);
		gpu_qpu_enable(0);
		panic("Failed to lock shared QPU memory\n");
	}

	gpu = (volatile struct GPU *)(uintptr_t)(bus_address & 0x3FFFFFFFU);
	gpu->handle = handle;
	return gpu;
}

static void gpu_destroy(volatile struct GPU *gpu)
{
	uint32_t handle = gpu->handle;

	if (gpu_unlock(handle) != 0)
		log("Failed to unlock shared QPU memory\n", LOG_ERROR);
	if (gpu_free(handle) != 0)
		log("Failed to release shared QPU memory\n", LOG_ERROR);
	if (gpu_qpu_enable(0) != 0)
		log("Failed to disable QPU\n", LOG_ERROR);
}

void gpu_test(void)
{
	FAT32_File shader_file;
	volatile struct GPU *gpu;
	uint32_t shader_size;
	int bytes_read;
	int result;
	unsigned int i;

	shader_file = fat32_open("/fib.bin");
	if (shader_file.first_cluster == 0 || shader_file.is_dir || shader_file.size == 0 || shader_file.size > GPU_SHADER_MAX_SIZE || (shader_file.size & 7U) != 0)
		panic("Could not open a valid /fib.bin QPU shader\n");
	
	shader_size = shader_file.size;
	gpu = gpu_create();
	for (i = 0; i < GPU_OUTPUT_WORDS; ++i)
		gpu->output[i] = 0;

	bytes_read = fat32_read(&shader_file, (uint8_t *)gpu->code, shader_size);
	fat32_close(&shader_file);
	if (bytes_read != (int)shader_size)
	{
		gpu_destroy(gpu);
		panic("Could not read complete /fib.bin QPU shader\n");
	}

	gpu->uniforms[0] = gpu_bus_address((const void *)gpu->output);
	gpu->mail[0] = gpu_bus_address((const void *)gpu->code);
	gpu->mail[1] = gpu_bus_address((const void *)gpu->uniforms);
	mmu_barrier();

	result = gpu_qpu_execute(1, gpu_bus_address((const void *)gpu->mail), 0, 10000);
	mmu_barrier();
	if (result != 0)
	{
		gpu_destroy(gpu);
		log("QPU execution failed (status %d)\n", LOG_ERROR, result);
		return;
	}

	log("Fibonacci shader result: %u (expected 55)\n", LOG_SUCCESS, gpu->output[0]);
	if (gpu->output[0] != 55)
		log("Unexpected Fibonacci result\n", LOG_ERROR);
	gpu_destroy(gpu);
}
