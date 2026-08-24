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

#define USER_HEAP_RESERVED			0x100000	/* 1 MiB of heap per process */
#define USER_HEAP_MAX(proc)			((proc)->heap_start + USER_HEAP_RESERVED)
#define KERNEL_STACK_PAGES			16
#define USER_STACK_PAGES			4
#define PAGE_SIZE					4096
#define STACK_CANARY				0xDEADC0DE

#define TIME_SLICE_MS				100 // delta t before each scheduler call

typedef enum {
	PROC_READY,
	PROC_RUNNING,
	PROC_BLOCKED,
	PROC_SLEEPING,
	PROC_ZOMBIE
}	e_process_state;

typedef enum e_process_mode
{
	PROCESS_KERNEL,
	PROCESS_USER
}	e_process_mode;

typedef struct s_process
{
	uint32_t		irq_sp;
	uint32_t		user_sp;

	uint32_t		pid;
	e_process_state	state;
	e_process_mode	mode;

	char			proc_name[64];

	void			*kernel_stack;
	void			*user_stack;

	uint32_t		priority;
	uint64_t		time_slice;

	uint32_t		heap_start;
	uint32_t		heap_end;
	void			*address_space;
	uint32_t		image_vaddr_base;
	uint32_t		image_size;

	struct s_process *next;
}	t_process;

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

#endif