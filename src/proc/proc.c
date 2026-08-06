#include "proc.h"

#define PAGE_SIZE		4096

uint32_t			current_pid = 0;


void	*alloc_pages(size_t pages)
{
	return ft_calloc(pages, PAGE_SIZE);
}

void prepare_initial_stack(t_process *p, void (*entry)(void))
{
    uint32_t *stack;

    stack = (uint32_t *)p->kernel_stack +
            (4 * PAGE_SIZE / sizeof(uint32_t));


    /*
     * irq_handler restores:
     *
     * ldmia sp!, {r0}
     * msr spsr_cxsf,r0
     * ldmia sp!, {r0-r12,lr}
     * subs pc,lr,#0
     */


    // lr restored by irq return
    *(--stack) = (uint32_t)entry;


    // r12-r0
    *(--stack) = 0; // r12
    *(--stack) = 0; // r11
    *(--stack) = 0; // r10
    *(--stack) = 0; // r9
    *(--stack) = 0; // r8
    *(--stack) = 0; // r7
    *(--stack) = 0; // r6
    *(--stack) = 0; // r5
    *(--stack) = 0; // r4
    *(--stack) = 0; // r3
    *(--stack) = 0; // r2
    *(--stack) = 0; // r1
    *(--stack) = 0; // r0


    // SPSR
    *(--stack) = 0x60000153;


    p->ctx.cpsr = (uint32_t)stack;
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
		log("  [%lu] %s t:0x%x sp:0x%x [priority=%lu]\n", 0, curr->pid, curr->proc_name, curr->state, curr->sp, curr->priority);
		curr = curr->next;
	}
}

t_process	*process_create(void (*entry)(void), char *name)
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
	p->state = PROC_READY;

	p->kernel_stack = alloc_pages(4);
	if (!p->kernel_stack)
	{
		free(p);
		return 0;
	}
	prepare_initial_stack(p, entry);

	scheduler_add(p);
	return p;
}