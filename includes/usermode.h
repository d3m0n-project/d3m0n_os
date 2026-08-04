#ifndef USER_MODE
#define USER_MODE

typedef int	(*syscall_t)(uint32_t, uint32_t, uint32_t, uint32_t);
typedef struct
{
	uint32_t	r0;
	uint32_t	r1;
	uint32_t	r2;
	uint32_t	r3;
	uint32_t	r4;
	uint32_t	r5;
	uint32_t	r6;
	uint32_t	r7;
	uint32_t	r8;
	uint32_t	r9;
	uint32_t	r10;
	uint32_t	r11;
	uint32_t	r12;
	uint32_t	lr;
}	syscall_frame_t;

void	test_func(void);
int		syscall_handler(syscall_frame_t *frame);

#endif