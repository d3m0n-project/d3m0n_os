#include "types.h"
#include "log.h"
#include "elf/elf.h"

extern void	enter_user(uint32_t entry, uint32_t stack);


#include "../../compiler/sdk/lib/sys.h"
void	test_code(void)
{
	print("test!\n");
	exit(0);
}

uint8_t		user_stack[4096] __attribute__((aligned(8)));
void test_func(void)
{
	log("launching in user mode\n", LOG_INFO);

	int fd = open("test_app", O_READ);
	if (fd < 0)
	{
		log("err\n", 0);
		while (1) {}
	}

	detect_type(fd);

	//uint8_t	code[] = {};

	enter_user((uint32_t)test_code, (uint32_t)(user_stack + sizeof(user_stack)));
	log("finished\n", LOG_INFO);
	while (1);
}