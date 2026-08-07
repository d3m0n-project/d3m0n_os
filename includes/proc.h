#ifndef PROC_H
#define PROC_H

#include "types.h"
#include "log.h"
#include "memory.h"
#include "libft.h"
#include "elf.h"

#define kernel_user_stack_offset	OFFSETOF(t_process, user_stack)

typedef enum {
	PROC_READY,
	PROC_RUNNING,
	PROC_BLOCKED,
	PROC_SLEEPING,
	PROC_ZOMBIE
}	e_process_state;

typedef struct s_process
{
	// CPU context
	uint32_t			sp;
	uint32_t			user_sp;
	

	uint32_t			pid;
	e_process_state		state;
	char				proc_name[64];


	// stacks
	void				*kernel_stack;
	void				*user_stack;

	// scheduling
	uint32_t			priority;
	uint64_t			time_slice;

	// memory
	uint32_t			heap_start;
	uint32_t			heap_end;
	void				*address_space;
	uint32_t			image_vaddr_base;
	uint32_t			image_size;

	struct s_process	*next;
}	t_process;

void		scheduler_start();
void		timer_handler();
t_process	*process_create(void (*entry)(void), char *name);

void		scheduler_add(t_process *p);
t_process	*scheduler_next();
void		scheduler_remove(t_process *p);
void		process_exit_current(uint32_t status_code);

void		process_list(void);

extern t_process		*scheduled_processes;
extern t_process		*current_process;
void					init_proc(void);

#endif