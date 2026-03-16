#include "time.h"
#include "types.h"

static inline uint64_t read_cntpct()
{
	uint64_t v;
	asm volatile("mrs %0, cntpct_el0" : "=r"(v));
	return v;
}

static inline uint64_t read_cntfrq()
{
	uint64_t v;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(v));
	return v;
}

uint64_t    time_now()
{
	return read_cntpct() / read_cntfrq();
}

uint64_t    time_us()
{
	uint64_t cnt = read_cntpct();
	uint64_t freq = read_cntfrq();
	return (cnt * 1000000) / freq;
}

void	sleep(uint64_t seconds)
{
	unsigned long long start = time_now();
	while (time_now() - start < seconds)
		asm volatile("wfe");
}

void usleep(uint64_t microseconds)
{
	uint64_t start = time_us();
	while ((time_us() - start) < microseconds)
		asm volatile("wfe");
}