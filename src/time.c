#include "time.h"
#include "types.h"

#define TIMER_BASE      0x20003000
#define TIMER_CLO       (*(volatile uint32_t*)(TIMER_BASE + 0x04))

static  uint64_t        current_time_delta = 0;

static inline uint32_t read_timer()
{
    return TIMER_CLO;
}

void    set_time(uint64_t time)
{
    current_time_delta = time;
}

uint64_t time_us()
{
    return current_time_delta + (uint64_t)read_timer();
}

void sleep(uint64_t seconds)
{
    uint64_t start = time_us();
    uint64_t target = seconds * 1000000ULL;

    while ((time_us() - start) < target)
        asm volatile("wfe");
}

void usleep(uint64_t microseconds)
{
    uint64_t start = time_us();

    while ((time_us() - start) < microseconds)
        asm volatile("wfe");
}