#include "d3m0n.h"
#include "log.h"
#include "dtb.h"
#include "display.h"
#include "filesystem.h"
#include "memory.h"


#include "time.h"
#include "peripheral.h"
#include "proc.h"
#include "IRQ.h"




t_conf		config;
t_conf	*get_config()
{
	return	&config;
}


void	show_kernel_status()
{
	log("KERNEL_VERSION:		%s\n", LOG_INFO| LOG_INDENT, KERNEL_VERSION);	
	log("KERNEL_VERSION_NAME:	%s\n", LOG_INFO | LOG_INDENT, KERNEL_VERSION_NAME);
}

void	panic(const char *message)
{
	log(message, LOG_ERROR);
	while (1)
		asm volatile("wfi");
}

void	kernel_main(void *dtb)
{
	log("Loading kernel...\n", LOG_INFO);
	show_kernel_status();

	// load DTB
	dtb_init(dtb);

	// init heap memory
	if (heap_init() != 0)		panic("Heap memory init failed\n");
	else						log("Heap memory initialized!\n", LOG_SUCCESS);


	// init fat32 filesystem
	if (sd_init() < 0)			panic("SD block interface init failed\n");
	else						log("SD block interface initialized!\n", LOG_SUCCESS);


	// load partition number 2 as rootfs
	if (fat32_mount(1) < 0)		panic("FAT32 mount failed\n");
	else
		log("FAT32 mounted!\n", LOG_SUCCESS);

	list_dir("/");

	
	// init framebuffer
	if (display_init())			panic("Could not initialize display\n");
	else						log("Display initialized!\n", LOG_SUCCESS);

	// init usb driver
	usb_init(); // TODO: maybe make usb driver optional and enabled for testing
	if (usb_enumerate() < 0)	log("USB enumeration did not find a configured root device\n", LOG_WARNING);

	// load spash
	BmpTexture	splash;
	if (bmp_load_image(&splash, "/wallpapers/splash.bmp"))
		log("Could not load splash screen\n", LOG_ERROR);
	else
		draw_bmp(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &splash, DISPLAY_COLORS[TRANSPARENT]);

	// parse config file
	if (parse_config(&config))	panic("Config parsing failed, please check config file\n");
	else						log("Config parsed successfully!\n", LOG_SUCCESS);


	timer_init();
	enable_irq();
	if (!process_create(init_proc, "init", 1))
		panic("Could not launch init process!\n");

	scheduler_start();


	log("Finished kernel!\n", LOG_WARNING);
	while (1) asm volatile ("wfi");
}
