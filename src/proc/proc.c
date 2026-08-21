#include "proc.h"

#define PAGE_SIZE			4096
#define USER_STACK_PAGES	4

uint32_t					current_pid = 0;

extern void		start_first_process(t_process *proc);


void	*alloc_pages(size_t pages)
{
	return ft_calloc(pages, PAGE_SIZE);
}

void	prepare_initial_stack(t_process *p, void (*entry)(void))
{
	uint32_t *stack;

	stack = (uint32_t *)p->kernel_stack + (KERNEL_STACK_PAGES * PAGE_SIZE / sizeof(uint32_t));
	stack -= 15;

	if (p->mode == PROCESS_USER)
		stack[0] = 0x10;
	else
		stack[0] = 0x53;

	for (int i = 1; i <= 13; i++)
		stack[i] = 0;

	stack[14] = (uint32_t)entry;

	p->irq_sp = (uint32_t)stack;
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
		log("  %s [%-7lu] %-15s [state:0x%x, irq sp:0x%x, priority=%lu]\n", 0, curr->mode?"U":"K", curr->pid, curr->proc_name, curr->state, curr->irq_sp, curr->priority);
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
	p->kernel_stack = alloc_pages(KERNEL_STACK_PAGES);
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

	prepare_initial_stack(p, entry);

	uint32_t *f = (uint32_t *)p->irq_sp;
	log("frame=%p\n", 0, f);
	log("SPSR=%x\n", 0, f[0]);
	log("r0=%x\n", 0, f[1]);
	log("r1=%x\n", 0, f[2]);
	log("lr=%x\n", 0, f[14]);

	scheduler_add(p);
	return p;
}

void process_exit_current(uint32_t status_code)
{
	t_process *exiting;
	t_process *next;

	log("Exited with status code: %llu\n", LOG_WARNING, status_code);

	exiting = current_process;

	if (!exiting)
		return;

	exiting->state = PROC_ZOMBIE;

	/*
	 * Find next BEFORE destroying the current relationship.
	 */
	next = exiting->next;

	if (!next)
		next = scheduled_processes;

	scheduler_remove(exiting);

	if (!next || next == exiting)
	{
		current_process = 0;

		log("No other process to resume\n", LOG_WARNING);

		while (1)
			;
	}
	
	current_process = next;
	log("SCHEDULE DEBUG current=%p sp=%x user_sp=%x mode=%x time=%llu\n",
		0,
		current_process,
		current_process->irq_sp,
		current_process->user_sp,
		current_process->mode,
		current_process->time_slice);
	current_process->state = PROC_RUNNING;
	current_process->time_slice = TIME_SLICE_MS;
}