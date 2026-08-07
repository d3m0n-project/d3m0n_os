#include "proc.h"

#define PAGE_SIZE			4096
#define USER_STACK_PAGES	4

uint32_t					current_pid = 0;

extern void		start_first_process(t_process *proc);


void	*alloc_pages(size_t pages)
{
	return ft_calloc(pages, PAGE_SIZE);
}

void prepare_initial_stack(t_process *p, void (*entry)(void))
{
	uint32_t *stack;

    stack = (uint32_t *)p->kernel_stack + (4 * PAGE_SIZE) / sizeof(uint32_t);

    *(--stack) = (uint32_t)entry;

    for (int i = 0; i < 13; i++)
        *(--stack) = 0;

    if (p->mode == PROCESS_USER)
        *(--stack) = 0x60000010;
    else
        *(--stack) = 0x60000013;

    p->sp = (uint32_t)stack;
}

uint32_t	allocate_pid()
{
	return current_pid++;
}

void		process_list(void)
{
	t_process	*curr = scheduled_processes;
	log("Process List:\n", 0);
	while (curr)
	{
		log("  %s [%lu] %s [state:0x%x, sp:0x%x, priority=%lu]\n", 0, curr->mode?"U":"K", curr->pid, curr->proc_name, curr->state, curr->sp, curr->priority);
		curr = curr->next;
	}
}

t_process	*process_create(void (*entry)(void), char *name, int kernel_mode)
{
	t_process *p = ft_calloc(sizeof(t_process), 1);
	if (!p)
	{
		log("PROC: Could not allocate new process\n", LOG_ERROR);
		return 0;
	}
	if (current_pid == (uint32_t)-1)
	{
		log("PROC: Maximum PID reached, kill some processes!\n", LOG_ERROR);
		return 0;
	}

	ft_strlcpy(p->proc_name, name, 64); // copy proc name

	p->pid = allocate_pid();
	p->state = PROC_READY; // TODO: priorities and state scheduling system
	p->kernel_stack = alloc_pages(4);
	if (kernel_mode)
		p->mode = PROCESS_KERNEL;
	else
	{
		p->mode = PROCESS_USER;

		p->user_stack = alloc_pages(USER_STACK_PAGES);
		if (!p->user_stack)
		{
			kfree(p->kernel_stack);
			return 0;
		}
		p->user_sp = ((uint32_t)p->user_stack + USER_STACK_PAGES * PAGE_SIZE) & ~7;
	}
	if (!p->kernel_stack || (!p->user_stack && p->mode == PROCESS_USER))
	{
		if (p->user_stack)
			kfree(p->user_stack);
		if (p->kernel_stack)
			kfree(p->kernel_stack);
		kfree(p);
		return 0;
	}
	p->user_sp = (uint32_t)p->user_stack + (USER_STACK_PAGES * PAGE_SIZE);
	p->user_sp &= ~7;

	prepare_initial_stack(p, entry);

	scheduler_add(p);
	return p;
}

void	process_exit_current(uint32_t status_code)
{
    t_process *exiting;

    log("Exited with status code: %llu\n", LOG_WARNING, status_code);
    exiting = current_process;
    if (!exiting)
        return;

    exiting->state = PROC_ZOMBIE;
    scheduler_remove(exiting);
    current_process = scheduler_next();
    if (!current_process)
    {
		log("No other process to resume\n", LOG_WARNING);
        while (1)
            ;
    }
    current_process->state = PROC_RUNNING;
}