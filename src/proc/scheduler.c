#include "proc.h"
#include "interrupts.h"
#include "d3m0n.h"

t_process				*scheduled_processes = 0;
t_process				*scheduled_processes_tail = 0;
t_process				*current_process = 0;

extern void	start_first_process(t_process *proc);

void	scheduler_add(t_process *p)
{
	if (!p)
		return;

	p->next = 0;
	p->time_slice = TIME_SLICE_MS;
	p->state = PROC_READY;
	if (!scheduled_processes)
	{
		scheduled_processes = p;
		scheduled_processes_tail = p;
		return;
	}

	scheduled_processes_tail->next = p;
	scheduled_processes_tail = p;
}

t_process	*scheduler_next(void)
{
	t_process	*next;

	if (!scheduled_processes)
		return 0;

	if (!current_process)
		return scheduled_processes;

	next = current_process->next;
	if (!next)
		next = scheduled_processes;

	return next;
}


void	scheduler_remove(t_process *p)
{
	t_process	*curr;

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
	curr = scheduled_processes;
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

void	schedule(void)
{
	t_process	*old;
	t_process	*next;

	old = current_process;
	next = scheduler_next();
	if (!next || next == old)
		return;

	if (old && old->state == PROC_RUNNING)
		old->state = PROC_READY;

	log("schedule to %s\n", 0, next->proc_name);

	next->state = PROC_RUNNING;
	next->time_slice = TIME_SLICE_MS;
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

	if (current_process->time_slice > 0)
		current_process->time_slice--;

	if (current_process->time_slice == 0)
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