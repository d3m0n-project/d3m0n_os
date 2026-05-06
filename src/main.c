#include "d3m0n.h"
#include "log.h"
#include "dtb.h"
#include "display.h"
#include "filesystem.h"
#include "memory.h"
#include "bmp.h"
#include "controls.h"
#include "parsing.h"
#include "time.h"
#include "peripheral.h"


void	show_kernel_status()
{
	log("KERNEL_VERSION:         %s\n", LOG_INFO| LOG_INDENT, KERNEL_VERSION);	
	log("KERNEL_VERSION_NAME:    %s\n", LOG_INFO | LOG_INDENT, KERNEL_VERSION_NAME);
}

void	panic(const char *message)
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

	// init heap memory
	if (heap_init() != 0)   panic("Heap memory init failed\n");
	else					log("Heap memory initialized!\n", LOG_SUCCESS);

	// init fat32 filesystem
	if (sd_init() < 0)		panic("SD block interface init failed\n");
	else					log("SD block interface initialized!\n", LOG_SUCCESS);

	if (fat32_mount() < 0)	log("FAT32 mount failed\n", LOG_ERROR);
	else					log("FAT32 mounted!\n", LOG_SUCCESS);

	// init framebuffer
	if (display_init())		panic("Could not initialize display\n");
	else					log("Display initialized!\n", LOG_SUCCESS);

	//list_dir("/");

	// load spash
	BmpTexture  texture;
	if (bmp_load_image(&texture, "wallpapers/default.bmp"))
		log("Could not load BMP texture\n", LOG_ERROR);
	else
		draw_bmp(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &texture);

	//sleep(3);

	if (!create_window(&main_window, "d3m0n home", SCREEN_WIDTH, SCREEN_HEIGHT))
		log("Main window created successfully!\n", LOG_SUCCESS);
	else	panic("Could not launch main window\n");
	main_window.bg_color = DISPLAY_COLORS[GREY];

	if (!parse_layout("test.layout", &main_window))
		log("Parsed layout successfully!\n", LOG_SUCCESS);
	else
		panic("Invalid layout, could not continue\n");

	if (!parse_source("test.src", &main_window))
		log("Parsed source file successfully!\n", LOG_SUCCESS);
	else
		log("Could not parse src file\n", LOG_ERROR);

	draw_window(&main_window);

	for (int i=0; i<10; i++)
	{
		if (main_window.events[i].type == EVENT_ON_CREATE) {
			exec_script(main_window.events[i].script);
		}
	}

	while (1)
	{
		usleep(200000);
	}

	log("Finished kernel!\n", LOG_WARNING);
	while (1)
		asm volatile ("wfi");
}