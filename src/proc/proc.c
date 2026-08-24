#include "proc.h"

uint32_t					current_pid = 0;

extern void		start_first_process(t_process *proc);


void	*alloc_pages(size_t pages)
{
	return ft_calloc(pages, PAGE_SIZE);
}

void	check_stack_canary(t_process *p)
{
	uint32_t *base = (uint32_t *)p->kernel_stack;
	if (base[0] != STACK_CANARY)
		log("!! STACK OVERFLOW on %s: canary=0x%x\n", LOG_ERROR, p->proc_name, base[0]);
}

void	prepare_initial_stack(t_process *p, void (*entry)(void))
{
	uint32_t *stack;

	stack = (uint32_t *)p->kernel_stack + (KERNEL_STACK_PAGES * PAGE_SIZE / sizeof(uint32_t));
	stack -= 15;
	
	((uint32_t *)p->kernel_stack)[0] = STACK_CANARY;
	

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
		process_dump_regs(curr);
		log("\n", 0);
		
		curr = curr->next;
	}
}

t_process *process_create(void (*entry)(void), char *name, int kernel_mode)
{
	t_process *p = ft_calloc(sizeof(t_process), 1);
	if (!p) {
		log("PROC: Could not allocate new process\n", LOG_ERROR);
		return 0;
	}
	if (current_pid == (uint32_t)-1) {
		log("PROC: Maximum PID reached, kill some processes!\n", LOG_ERROR);
		kfree(p);
		return 0;
	}

	ft_strlcpy(p->proc_name, name, 64);
	p->pid = allocate_pid();
	p->state = PROC_READY;
	p->kernel_stack = alloc_pages(KERNEL_STACK_PAGES);
	if (kernel_mode)
	{
		p->mode = PROCESS_KERNEL;
		p->user_sp = ((uint32_t)p->kernel_stack + KERNEL_STACK_PAGES * PAGE_SIZE) & ~7;
	}
	else
	{
		p->mode = PROCESS_USER;
		p->user_stack = alloc_pages(USER_STACK_PAGES);
		if (!p->user_stack)
		{
			kfree(p->kernel_stack);
			kfree(p);
			return 0;
		}
		p->user_sp = ((uint32_t)p->user_stack + USER_STACK_PAGES * PAGE_SIZE) & ~7;
	}
	
	if (!p->kernel_stack)
	{
		if (p->user_stack)
			kfree(p->user_stack);
		kfree(p->kernel_stack);
		kfree(p);
		return 0;
	}

	prepare_initial_stack(p, entry);
	scheduler_add(p);
	return p;
}

void process_exit_current(uint32_t status_code)
{
	log("Exited with status code: %lu\n", LOG_WARNING, status_code);

	uint32_t cpsr = disable_interrupts();
	t_process *exiting = current_process;
	if (!exiting)
	{
		restore_interrupts(cpsr);
		return;
	}

	exiting->state = PROC_ZOMBIE;
	scheduler_remove(exiting);
	if (!scheduled_processes)
	{
		current_process = 0;
		log("No other process to resume\n", LOG_WARNING);
		while (1)
			__asm__ volatile("wfe");
	}

	exiting->time_slice = 0;
	restore_interrupts(cpsr);
	while (1)
		__asm__ volatile("wfe");
}