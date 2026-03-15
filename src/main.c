#include "d3m0n.h"
#include "log.h"
#include "dtb.h"
#include "display.h"
#include "filesystem.h"

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
    display_init();

    // init fat32 filesystem
    if (sd_init() < 0)
        log("SD block interface init failed\n", LOG_ERROR);
    else
    {
        log("SD block interface initialized!\n", LOG_SUCCESS);
        if (fat32_mount() < 0)
            log("FAT32 mount failed\n", LOG_ERROR);
        else
        {
            log("FAT32 mounted!\n", LOG_SUCCESS);
            int fd = open("test.txt", O_CREATE | O_WRITE);
            if (fd < 0)
                log("file not found: test.txt\n", LOG_ERROR);
            else
            {
                write(fd, "Hello World!", 12);
                close(fd);
            }
            fd = open("test.txt", O_READ);
            if (fd < 0)
                log("file not found: test.txt\n", LOG_ERROR);
            else
            {
                char    buffer[20] = {0};
                int len = read(fd, buffer, 12);
                log("file: '%s', %i\n", LOG_ERROR, buffer, len);
                close(fd);
            }
        }
    }


    log("Finished kernel!\n", LOG_WARNING);

    while(1);
}