#include "d3m0n.h"
#include "log.h"
#include "time.h"
#include "uart.h"

void    show_kernel_status()
{
    log("         KERNEL_VERSION:          %s\n", LOG_NONE, KERNEL_VERSION);    
    log("         KERNEL_VERSION_NAME:     %s\n", LOG_NONE, KERNEL_VERSION_NAME);
}

void kernel_main()
{
    log("Loading kernel...\n", LOG_INFO, NULL);
    show_kernel_status();

    //log("Timestamp: %llu\n", LOG_INFO, time_now(), NULL);

    log("Finished kernel!\n", LOG_WARNING, NULL);

    while(1);
}