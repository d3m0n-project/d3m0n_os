#include "d3m0n.h"
#include "log.h"

void    show_kernel_status()
{
    log("         KERNEL_VERSION:          ", LOG_NONE);
    log(KERNEL_VERSION, LOG_NONE);
    
    log("\n         KERNEL_VERSION_NAME:     ", LOG_NONE);
    log(KERNEL_VERSION_NAME, LOG_NONE);

    log("\n", LOG_NONE);
}

void kernel_main()
{
    log("Loading kernel...\n", LOG_INFO);
    show_kernel_status();

    log("Finished kernel!\n", LOG_WARNING);

    while(1);
}