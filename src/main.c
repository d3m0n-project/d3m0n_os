#include "d3m0n.h"
#include "log.h"
#include "dtb.h"
#include "display.h"
#include "mailbox.h"

void    show_kernel_status()
{
    log("         KERNEL_VERSION:          %s\n", LOG_NONE, KERNEL_VERSION);    
    log("         KERNEL_VERSION_NAME:     %s\n", LOG_NONE, KERNEL_VERSION_NAME);
}

void kernel_main(void *dtb)
{
    log("Loading kernel...\n", LOG_INFO);

    show_kernel_status();

    // load DTB
    dtb_init(dtb);
    
    // init framebuffer
    if(!framebuffer_init(640, 480, 32))
    {
        log("Framebuffer allocated!\n", LOG_SUCCESS);
        for(uint32_t y=0; y<fb_req.height; y++)
            for(uint32_t x=0; x<fb_req.width; x++)
                put_pixel(x, y, 0x000000FF);
    } else {
        log("Framebuffer allocation failed!\n", LOG_ERROR);
    }


    log("Finished kernel!\n", LOG_WARNING);

    while(1);
}