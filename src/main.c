#include "d3m0n.h"
#include "log.h"
#include "display.h"

void    show_kernel_status()
{
    log("         KERNEL_VERSION:          %s\n", LOG_NONE, KERNEL_VERSION);    
    log("         KERNEL_VERSION_NAME:     %s\n", LOG_NONE, KERNEL_VERSION_NAME);
}

void kernel_main()
{
    log("Loading kernel...\n", LOG_INFO);
    show_kernel_status();

    //put_pixel(10, 10, 0x00FF0000);
    #define fb_loc 0x40002000
    uint32_t* fb_address = (uint32_t*)fb_loc;
    for (int i = 0; i < 800*600; i++) {
        fb_address[i] = 0xFFFF0000;
    }

    log("Finished kernel!\n", LOG_WARNING);

    while(1);
}