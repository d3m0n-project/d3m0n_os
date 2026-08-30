#include "watchdog.h"

#define PM_BASE     0x20100000
#define PM_WDOG     (*(volatile uint32_t*)(PM_BASE + 0x24))
#define PM_RSTC     (*(volatile uint32_t*)(PM_BASE + 0x1c))
#define PM_PASSWORD 0x5A000000

void    watchdog_feed(void)
{
    PM_WDOG = PM_PASSWORD | 0xFFFFF; // reset timer to max
    PM_RSTC = PM_PASSWORD | (PM_RSTC & 0x30); // keep reset type, clear trigger
}