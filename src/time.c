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

uint64_t time_now()
{
    return read_cntpct() / read_cntfrq();
}

void    sleep(int seconds)
{
    unsigned long long start = time_now();
    while (time_now() - start < (unsigned long long)seconds)
        continue;
}