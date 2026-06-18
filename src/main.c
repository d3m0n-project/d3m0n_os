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
#include "settings.h"

#include "spi.h"


void	show_kernel_status()
{
	log("KERNEL_VERSION:         %s\n", LOG_INFO| LOG_INDENT, KERNEL_VERSION);	
	log("KERNEL_VERSION_NAME:    %s\n", LOG_INFO | LOG_INDENT, KERNEL_VERSION_NAME);
}

void	panic(const char *message)
{
	log(message, LOG_ERROR);
	/* ensure any buffered log data is flushed to disk for post-mortem */
	uart_flush_log_buffer();
	while (1)
		asm volatile("wfi");
}

void kernel_main(void *dtb)
{
	t_window	main_window;
	t_conf		config;

	log("Loading kernel...\n", LOG_INFO);

	show_kernel_status();

	// load DTB
	dtb_init(dtb);

	// init heap memory
	if (heap_init() != 0)      panic("Heap memory init failed\n");
	else					   log("Heap memory initialized!\n", LOG_SUCCESS);


	// init fat32 filesystem
	if (sd_init() < 0)		   panic("SD block interface init failed\n");
	else					   log("SD block interface initialized!\n", LOG_SUCCESS);


	// load partition number 2 as rootfs
	if (fat32_mount(1) < 0)	   panic("FAT32 mount failed\n");
	else
	{
		log_cleanup(); // cleanup the log file to remove old boots logs
		uart_enable_file_logging();
		log("FAT32 mounted!\n", LOG_SUCCESS);
	}

	if (parse_config(&config)) panic("Config parsing failed, please check config file\n");
	else					   log("Config parsed successfully!\n", LOG_SUCCESS);

	list_dir("/");

	//spi_init(10000);
	//char	*text = "Hello World!\n";
	//while (1)
	//{
	//	spi_write_buffer(text, 13);
	//	uart_print(text);
	//	sleep(1);
	//}

	// init framebuffer
	if (display_init())		   panic("Could not initialize display\n");
	else					   log("Display initialized!\n", LOG_SUCCESS);


	// load spash
	BmpTexture  texture;
	if (bmp_load_image(&texture, "wallpapers/default.bmp")) // TODO: change me
		log("Could not load BMP texture\n", LOG_ERROR);
	//else
	//	draw_bmp(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &texture);


	//panic("STOPPED THE EXECUTION HERE\n");

	sleep(3);
	
	if (!parse_manifest("/apps/system/desktop/source/app", &main_window))
		log("Main window created successfully!\n", LOG_SUCCESS);
	else	panic("Could not create main window\n");
	main_window.bg_color = DISPLAY_COLORS[GREY]; // TODO: parse color of window when parsing

	if (!parse_layout("/apps/system/desktop/source/layouts/main.layout", &main_window, 0, 0, 0))
		log("Parsed layout successfully!\n", LOG_SUCCESS);
	else
		panic("Invalid layout, could not continue\n");

	if (!parse_source("/apps/system/desktop/source/src/main.src", &main_window))
		log("Parsed source file successfully!\n", LOG_SUCCESS);
	else
		panic("Could not parse src file\n");

	exec_event(0, EVENT_ON_CREATE, &main_window); // Window.OnCreate

	while (1)
		usleep(200000);

	log("Finished kernel!\n", LOG_WARNING);
	while (1) asm volatile ("wfi");
}
