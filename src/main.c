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

// TODO: remove me
#include "gpio.h"
#include "spi.h"
#include "lcd.h"


#include "package_manager.h"

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
	uart_flush_log_buffer();
	while (1)
		asm volatile("wfi");
}

void	kernel_main(void *dtb)
{
	t_window	main_window;

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
	{
		log_cleanup(); // cleanup the log file to remove old boots logs
		uart_enable_file_logging();
		log("FAT32 mounted!\n", LOG_SUCCESS);
	}

	list_dir("/");

	// init framebuffer
	if (display_init())			panic("Could not initialize display\n");
	else						log("Display initialized!\n", LOG_SUCCESS);

	// init usb driver
	usb_init(); // TODO: maybe make usb driver optional and enabled for testing
	if (usb_enumerate() < 0)	log("USB enumeration did not find a configured root device\n", LOG_WARNING);

	// parse config file
	if (parse_config(&config))	panic("Config parsing failed, please check config file\n");
	else						log("Config parsed successfully!\n", LOG_SUCCESS);


	if (load_app_list())		panic("Failed to load the apps list\n");
	else						log("Loaded apps successfully!\n", LOG_SUCCESS);

	// load spash
	BmpTexture	splash;
	if (bmp_load_image(&splash, "wallpapers/splash.bmp"))
		log("Could not load splash screen\n", LOG_ERROR);
	else
		draw_bmp(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, &splash, DISPLAY_COLORS[TRANSPARENT]);

	usleep(config.splash_time * 1000);
	

	// load desktop app manifest
	char	*manifest = get_app_path_from_package(config.launcher, PACKAGE_MANIFEST);
	if (manifest && !parse_manifest((const char *)manifest, &main_window))
	{
		free(manifest);
		log("Main window created successfully!\n", LOG_SUCCESS);
	}
	else	panic("Could not create main window\n");

	// set window launcher mode
	main_window.is_launcher = 1;

	main_window.bg_color = DISPLAY_COLORS[GREY]; // TODO: parse color of window when parsing

	char *main_layout = get_app_path_from_package(config.launcher, PACKAGE_MAIN_LAYOUT);
	if (main_layout && !parse_layout(main_layout, &main_window, 0, 0, 0))
	{
		free(main_layout);
		log("Parsed layout successfully!\n", LOG_SUCCESS);
	}
	else if (!main_layout)
		panic("Could not find main layout\n");
	else
		panic("Invalid layout, could not continue\n");


	char *main_source = get_app_path_from_package(config.launcher, PACKAGE_MAIN_SOURCE);
	if (!parse_source(main_source, &main_window, 0))
	{
		free(main_source);
		log("Parsed source file successfully!\n", LOG_SUCCESS);
	}
	else if (!main_source)
		panic("Could not find main source\n");
	else
		panic("Could not parse src file\n");



	exec_event(0, EVENT_ON_CREATE, &main_window); // Window.OnCreate

	while (1)
	{
		usb_mouse_poll();
		usleep(20000);
	}

	log("Finished kernel!\n", LOG_WARNING);
	while (1) asm volatile ("wfi");
}
