#ifndef PROC_H
#define PROC_H

#include "types.h"
#include "log.h"
#include "memory.h"
#include "libft.h"
#include "elf.h"

typedef enum {
	PROC_READY,
	PROC_RUNNING,
	PROC_BLOCKED,
	PROC_SLEEPING,
	PROC_ZOMBIE
}	e_process_state;

typedef struct s_cpu_context
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;

	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
}	t_cpu_context;

typedef struct s_process
{
	// CPU context
	t_cpu_context		ctx;
	uint32_t			lr;

	uint32_t			pid;
	e_process_state		state;
	char				proc_name[64];


	// kernel stack
	void				*kernel_stack;

	// scheduling
	uint32_t			priority;
	uint64_t			time_slice;

	// memory
	void				*address_space;

	struct s_process	*next;
}	t_process;

void		scheduler_start();
void		timer_handler();
t_process	*process_create(void (*entry)(void), char *name);

void		scheduler_add(t_process *p);
t_process	*scheduler_next();
void		scheduler_remove(t_process *p);

void		process_list(void);

extern t_process		*scheduled_processes;


#endif