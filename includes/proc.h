#ifndef PROC_H
#define PROC_H

#include "types.h"
#include "log.h"
#include "memory.h"
#include "libft.h"
#include "elf.h"
#include "d3m0n.h"
#include "interrupts.h"

#define kernel_user_stack_offset	OFFSETOF(t_process, user_stack)

#define USER_HEAP_RESERVED			0x100000
#define USER_HEAP_MAX(proc)			((proc)->heap_start + USER_HEAP_RESERVED)
#define KERNEL_STACK_PAGES			16
#define USER_STACK_PAGES			16
#define PAGE_SIZE					4096
#define STACK_CANARY				0xDEADC0DE
#define IRQ_FRAME_WORDS			15
#define IRQ_FRAME_SIZE			(IRQ_FRAME_WORDS * sizeof(uint32_t))

#define TIME_SLICE_MS				5 // delta t before each scheduler call

typedef uint32_t e_process_state;
#define PROC_READY			0
#define PROC_RUNNING		1
#define PROC_BLOCKED		2
#define PROC_SLEEPING		3
#define PROC_ZOMBIE			4

typedef uint32_t e_process_mode;
#define PROCESS_KERNEL		0
#define PROCESS_USER		1


typedef struct s_process
{
	uint32_t		irq_sp;
	uint32_t		user_sp;

	uint32_t		user_lr;


	uint32_t		pid;
	e_process_state	state;
	e_process_mode	mode;

	char			proc_name[64];

	void			*kernel_stack;
	void			*user_stack;

	uint32_t		priority;
	uint32_t		time_slice;

	uint32_t		heap_start;
	uint32_t		heap_end;
	void			*address_space;
	uint32_t		image_vaddr_base;
	uint32_t		image_size;
	uint32_t		surface_addr;
	uint32_t		surface_size;
	uint32_t		surface_width;
	uint32_t		surface_height;

	struct s_process *next;
}	t_process;

_Static_assert(OFFSETOF(t_process, irq_sp)       == 0,   "irq_sp");
_Static_assert(OFFSETOF(t_process, user_sp)      == 4,   "user_sp");
_Static_assert(OFFSETOF(t_process, user_lr)      == 8,   "user_lr");
_Static_assert(OFFSETOF(t_process, pid)          == 12,  "pid");
_Static_assert(OFFSETOF(t_process, state)        == 16,  "state");
_Static_assert(OFFSETOF(t_process, mode)         == 20,  "mode");
_Static_assert(OFFSETOF(t_process, proc_name)    == 24,  "proc_name");
_Static_assert(OFFSETOF(t_process, kernel_stack) == 88,  "kernel_stack");
_Static_assert(OFFSETOF(t_process, user_stack)   == 92,  "user_stack");
_Static_assert(OFFSETOF(t_process, priority)     == 96,  "priority");
_Static_assert(OFFSETOF(t_process, time_slice)   == 100, "time_slice");

void					scheduler_start();
void					timer_handler();
t_process				*process_create(void (*entry)(void), char *name, int kernel_mode);

void					scheduler_add(t_process *p);
t_process				*scheduler_next();
void					scheduler_remove(t_process *p);
void					process_exit_current(uint32_t status_code);

void					process_list(void);

extern t_process		*scheduled_processes;
extern t_process		*scheduled_processes_tail;
extern t_process		*current_process;
void					init_proc(void);

// debug
void					process_dump_regs(t_process *p);

// security
void					check_stack_canary(t_process *p);
int					process_context_valid(t_process *p);

#endif
