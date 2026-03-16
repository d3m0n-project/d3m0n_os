#include "d3m0n.h"
#include "log.h"
#include "dtb.h"
#include "display.h"
#include "filesystem.h"
#include "memory.h"
#include "bmp.h"
#include "controls.h"

void	show_kernel_status()
{
	log("    KERNEL_VERSION:        %s\n", LOG_NONE, KERNEL_VERSION);	
	log("    KERNEL_VERSION_NAME:   %s\n", LOG_NONE, KERNEL_VERSION_NAME);
}

void	hang(const char *message)
{
	log(message, LOG_ERROR);
	while (1)
		asm volatile("wfi");
}

void kernel_main(void *dtb)
{
	t_window	main_window;

	log("Loading kernel...\n", LOG_INFO);

	show_kernel_status();

	// load DTB
	dtb_init(dtb);
	
	// init framebuffer
	display_init();

	// init heap memory
	if (heap_init() != 0)   hang("Heap memory init failed\n");
	else					log("Heap memory initialized!\n", LOG_SUCCESS);

	// init fat32 filesystem
	if (sd_init() < 0)		hang("SD block interface init failed\n");
	else					log("SD block interface initialized!\n", LOG_SUCCESS);

	if (fat32_mount() < 0)	log("FAT32 mount failed\n", LOG_ERROR);
	else					log("FAT32 mounted!\n", LOG_SUCCESS);

	list_dir("/");

	// load spash
	BmpTexture  texture;
	if (bmp_load_image(&texture, "wallpapers/default.bmp"))
		log("Could not load BMP texture\n", LOG_ERROR);
	else
		draw_bmp(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &texture);


	if (!create_window(&main_window, "d3m0n home", SCREEN_WIDTH, SCREEN_HEIGHT))
		log("Main window created successfully!\n", LOG_SUCCESS);
	else	hang("Could not launch main window\n");
	
	log("Finished kernel!\n", LOG_WARNING);
	while(1) asm volatile("wfi");
}