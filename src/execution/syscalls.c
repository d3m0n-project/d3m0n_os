#include "types.h"
#include "log.h"
#include "usermode.h"

int	sys_exit(uint32_t mode, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	log("exited with status code: %llu\n", LOG_WARNING, mode);
	return (int)mode;
}

int	sys_print(uint32_t str, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	log((char *)str, LOG_NONE);
	return 0;
}



syscall_t	syscall_table[] = {
	sys_exit,
	sys_print
};

int	syscall_dispatch(uint32_t number, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	if (number >= sizeof(syscall_table)/sizeof(syscall_table[0]))
		return -1;
	return syscall_table[number](a0, a1, a2, a3);
}

int	syscall_handler(syscall_frame_t *frame)
{
	int ret = syscall_dispatch(frame->r7, frame->r0, frame->r1, frame->r2, frame->r3);
	if (frame->r7 == 0)
		return 1;

	frame->r0 = ret;
	return 0;
}