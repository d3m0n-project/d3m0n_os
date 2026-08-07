#include "proc.h"
#include "time.h"
#include "crypto.h"
#include "d3m0n.h"
#include "bmp.h"
#include "display.h"
#include "package_manager.h"
#include "controls.h"
#include "icons.h"
#include "parsing.h"
#include "settings.h"
#include "usb.h"
#include "../compiler/sdk/lib/sys.h"

void	rsa_require_thread(void)
{
	// rsa test
	t_RSA_private_key	prv;
	t_RSA_public_key	pub;
	if (!rsa_private_key_require("/security/private.key", &prv, &pub))
		panic("Failed to get your RSA keypair!\n");
	
	size_t	em_len = 0;
	uint8_t	*em = pkcs1_v1_5_generate_em(&pub, (uint8_t *)"Hello World!", 12, &em_len);
	BigInt *c = rsa_encrypt(em, em_len, &pub);
	if (!c)
		log("Failed to encrypt RSA\n", LOG_ERROR);
	size_t	out_len = 0;
	uint8_t	*m = rsa_decrypt(c, &out_len, &prv);
	if (!m)
		log("Failed to decrypt RSA\n", LOG_ERROR);
	uint8_t	*msg = pkcs1_v1_5_decode_em(m, out_len, &out_len);
	if (!msg)
		log("Failed to decrypt pkcs#1 v1.5\n", LOG_ERROR);

	log("msg bytes: ", 0);
	for (size_t i=0; i<out_len; i++)
		log("%X ", 0, msg[i]);
	log("\n", 0);
	log("msg: '", 0);
	for (size_t i=0; i<out_len; i++)
		log("%c", 0, msg[i]);
	log("'\n", 0);
}

void	app_and_icon_loader_thread(void)
{
	if (load_app_list())		panic("Failed to load the apps list\n");
	else						log("Loaded apps successfully!\n", LOG_SUCCESS);

	t_conf	*config = get_config();

	uint64_t t = time_us();
	char		*icon_pack_path = path_add("/themes/", config->icon_pack);
	if (!icon_pack_path || load_icon_pack(icon_pack_path))
		log("Could not load icon pack: %s\n", LOG_ERROR, icon_pack_path);
	else
		log("Loaded icons in %ims\n", LOG_INFO, (time_us() - t) / 1000);
	if (icon_pack_path)
		kfree(icon_pack_path);	
}


void	init_proc(void)
{
	t_conf		*config = get_config();
	t_window	main_window;

	log("[init] started at %llums\n", LOG_SUCCESS, time_us() / 1000);

	//elf_to_proc("test_app"); // TODO: change me
	process_list();

	
	process_create(rsa_require_thread, "[SECURITY]RSA");


	process_create(app_and_icon_loader_thread, "[LOADER]app+icon");
	
	
	
	// load desktop app manifest
	char	*manifest = get_app_path_from_package(config->launcher, PACKAGE_MANIFEST);
	if (manifest && !parse_manifest((const char *)manifest, &main_window))
	{
		kfree(manifest);
		log("Main window created successfully!\n", LOG_SUCCESS);
	}
	else	panic("Could not create main window\n");

	// set window launcher mode
	main_window.is_launcher = 1;

	main_window.bg_color = DISPLAY_COLORS[GREY]; // TODO: parse color of window when parsing

	char *main_layout = get_app_path_from_package(config->launcher, PACKAGE_MAIN_LAYOUT);
	if (main_layout && !parse_layout(main_layout, &main_window, 0, 0, 0, 0))
	{
		kfree(main_layout);
		log("Parsed layout successfully!\n", LOG_SUCCESS);
	}
	else if (!main_layout)
		panic("Could not find main layout\n");
	else
		panic("Invalid layout, could not continue\n");


	char *main_source = get_app_path_from_package(config->launcher, PACKAGE_MAIN_SOURCE);
	if (!parse_source(main_source, &main_window, 0))
	{
		kfree(main_source);
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
}