#include "proc/proc.h"

extern void		start_first_process(t_process *proc);
uint32_t	current_pid = 0;

void	*alloc_pages(size_t pages)
{
	return ft_calloc(pages, PAGE_SIZE);
}

void	check_stack_canary(t_process *p)
{
	if (!p || !p->kernel_stack)
		return;
	uint32_t *base = (uint32_t *)p->kernel_stack;
	if (base[0] != STACK_CANARY)
	{
		log("!! STACK OVERFLOW on %s: canary=0x%x\n", LOG_ERROR, p->proc_name, base[0]);
		panic("PROCESS: kernel stack corruption\n");
	}
}

void	prepare_initial_stack(t_process *p, void (*entry)(void))
{
	uint32_t *stack;

	stack = (uint32_t *)p->kernel_stack + (KERNEL_STACK_PAGES * PAGE_SIZE / sizeof(uint32_t));
	stack -= IRQ_FRAME_WORDS;
	
	((uint32_t *)p->kernel_stack)[0] = STACK_CANARY;
	

	if (p->mode == PROCESS_USER)
		stack[0] = 0x10;
	else
		stack[0] = 0x53;

	for (int i = 1; i <= 13; i++)
		stack[i] = 0;

	stack[14] = (uint32_t)entry;

	p->irq_sp = (uint32_t)stack;
	p->user_lr = 0;
}

int process_context_valid(t_process *p)
{
	uint32_t	low;
	uint32_t	high;
	uint32_t	*f;
	uint32_t	mode;

	if (!p || !p->kernel_stack || !p->irq_sp)
		return 0;

	low  = (uint32_t)p->kernel_stack;
	high = low + KERNEL_STACK_PAGES * PAGE_SIZE;

	if (p->irq_sp < low || p->irq_sp > high - IRQ_FRAME_SIZE || (p->irq_sp & 3))
		return 0;

	if (p->user_sp & 3)
		return 0;
	if (p->mode != PROCESS_KERNEL && p->mode != PROCESS_USER)
		return 0;

	if (p->mode == PROCESS_USER)
	{
		uint32_t user_low;
		uint32_t user_high;

		if (!p->user_stack)
			return 0;
		user_low  = (uint32_t)p->user_stack;
		user_high = user_low + USER_STACK_PAGES * PAGE_SIZE;
		if (p->user_sp < user_low + 4 || p->user_sp > user_high)
			return 0;
	}
	else
	{
		if (p->user_sp < low + 4 || p->user_sp > high)
			return 0;
	}

	f	= (uint32_t *)p->irq_sp;
	mode = f[0] & 0x1f;

	if (p->mode == PROCESS_KERNEL && mode != 0x13)
		return 0;
	if (p->mode == PROCESS_USER && mode != 0x1f && mode != 0x10)
		return 0;

	if (f[14] & 3)
		return 0;

	return 1;
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
		//process_dump_regs(curr);
		//log("\n", 0);
		
		curr = curr->next;
	}
}

t_process *process_create(void (*entry)(void), char *name, int kernel_mode)
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
		kfree(p);
		return 0;
	}

	ft_strlcpy(p->proc_name, name, 64);
	p->pid = allocate_pid();
	p->state = PROC_READY;
	p->kernel_stack = alloc_pages(KERNEL_STACK_PAGES);
	if (!p->kernel_stack)
	{
		kfree(p);
		return 0;
	}

	if (kernel_mode)
	{
		p->mode = PROCESS_KERNEL;
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

	for (int i = 0; i < FS_MAX_FDS; i++)
		p->fds[i] = (fs_fd){0};

	prepare_initial_stack(p, entry);

	// for kernel processes, user_sp is the SVC sp — initialize it
	// to irq_sp so the validator sees a consistent in-bounds value
	// before the first context save
	if (kernel_mode)
		p->user_sp = p->irq_sp;

	scheduler_add(p);
	return p;
}

void 	process_exit_current(uint32_t status_code)
{
	uint32_t	cpsr;
	t_process   *exiting;
	uint32_t	surface_addr;
	t_process   *next;

	cpsr = disable_interrupts();
	exiting = current_process;
	if (!exiting)
	{
		restore_interrupts(cpsr);
		return;
	}

	log("exiting proc: 0x%x\n", 1, exiting);
	surface_addr = exiting->surface_addr;
	if (surface_addr)
	{
		exiting->surface_addr = 0;
		kfree((void *)(uintptr_t)surface_addr);
	}

	if (status_code >= (uint32_t)-6)
	{
		log("Crashed '%s' due to a %s\n", LOG_ERROR, exiting->proc_name, get_exception_name(-status_code));
		exiting->state = PROC_CRASHED;
	}
	else
	{
		log("Exited '%s' with status code: %lu\n", LOG_WARNING, exiting->proc_name, status_code);
		exiting->state = PROC_ZOMBIE;
	}

	for (int i = 0; i < FS_MAX_FDS; i++)
	{
		if (exiting->fds[i].file.first_cluster)
			fat32_close(&exiting->fds[i].file);
	}

	scheduler_remove(exiting);

	next = scheduler_next();
	if (!next)
	{
		current_process = 0;
		log("SCHEDULER: No other process to resume\n", LOG_WARNING);
		restore_interrupts(cpsr & ~(1u << 7));
		while (1)
			__asm__ volatile("wfe");
	}

	next->state = PROC_RUNNING;
	next->time_slice = TIME_SLICE_MS;
	current_process = next;

	mmu_switch_current();
	start_first_process(next);
}