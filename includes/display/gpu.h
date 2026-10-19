#ifndef GPU_H
#define GPU_H

int		gpu_alloc(uint32_t size, uint32_t align);

int		gpu_lock(uint32_t handle, uint32_t *bus_addr);
int		gpu_unlock(void);

int		gpu_execute_code(const uint32_t code, const uint32_t r0, const uint32_t r1, const uint32_t r2, const uint32_t r3, const uint32_t r4, const uint32_t r5);
int		gpu_qpu_execute(const uint32_t num_qpus, const uint32_t control, const uint32_t noflush, const uint32_t timeout);

int		gpu_qpu_enable(const uint32_t enable);

#endif