#include "proc.h"
#include "IRQ.h"
#include "d3m0n.h"

t_process				*scheduled_processes = 0;
static t_process		*scheduled_processes_tail = 0;
t_process				*current_process = 0;
#define TIME_SLICE_MS	1

extern void	start_first_process(t_process *proc);

void	scheduler_add(t_process *p)
{
	if (!p)
		return;

	p->next = 0;
	p->time_slice = TIME_SLICE_MS;
	if (!scheduled_processes)
	{
		scheduled_processes = p;
		scheduled_processes_tail = p;
		return;
	}
	scheduled_processes_tail->next = p;
	scheduled_processes_tail = p;
}

t_process	*scheduler_next()
{
	t_process *curr = scheduled_processes;

	if (!curr)
		return 0;

	if (!curr->next)
		return curr;

	scheduled_processes = curr->next;
	curr->next = 0;
	scheduled_processes_tail->next = curr;
	scheduled_processes_tail = curr;
	return scheduled_processes;
}

void	scheduler_remove(t_process *p)
{
	if (!p || !scheduled_processes)
		return;

	if (scheduled_processes == p)
	{
		scheduled_processes = p->next;
		p->next = 0;
		if (!scheduled_processes)
			scheduled_processes_tail = 0;
		return;
	}

	t_process *curr = scheduled_processes;
	while (curr->next)
	{
		if (curr->next == p)
		{
			curr->next = p->next;
			p->next = 0;
			if (scheduled_processes_tail == p)
				scheduled_processes_tail = curr;
			return;
		}
		curr = curr->next;
	}
}

void schedule(void)
{
	t_process *old = current_process;

	//log("switch %s -> ", 0, current_process->proc_name, 0);
	if (old)
		old->state = PROC_READY;

	t_process *next = scheduler_next();

	//log("%s\n", 0, next->proc_name, 0);

	if (!next)
		return;
		
	next->time_slice = TIME_SLICE_MS;
	next->state = PROC_RUNNING;
	current_process = next;
}



void scheduler_start()
{
	current_process = scheduler_next();
	if (!current_process)
	{
		panic("SCHEDULER: No init process!\n");
		return;
	}

	current_process->state = PROC_RUNNING;

	// jump to first process
	start_first_process(current_process);
}

void timer_handler(void)
{
	timer_ack();
	if (!current_process)
		return;

	//log("%s sp=%x\n", 0,
    //    current_process->proc_name,
    //    current_process->sp);

	if (--current_process->time_slice <= 0)
	{
		current_process->time_slice = TIME_SLICE_MS;
		schedule();
	}
}

void irq_dispatch(void)
{
	uint32_t pending = IRQ_PENDING_1;

	if (pending & (1 << 1))
	{
		ST_CS = (1 << 1);

		timer_handler();
	}
}