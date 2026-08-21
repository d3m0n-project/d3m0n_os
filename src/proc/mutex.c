#include "mutex.h"
#include "interrupts.h"

void	mutex_init(t_mutex *mutex)
{
	if (!mutex)
		return;
	mutex->locked = 0;
	mutex->irq_state = 0;
}

void	mutex_lock(t_mutex *mutex)
{
	if (!mutex)
		return;

	uint32_t state;
	asm volatile("mrs %0, cpsr" : "=r"(state));
	mutex->irq_state = (state & 0x80) ? 0 : 1;

	disable_irq();
	while (mutex->locked)
		;
	mutex->locked = 1;
}

void	mutex_unlock(t_mutex *mutex)
{
	if (!mutex)
		return;

	mutex->locked = 0;
	if (mutex->irq_state)
		enable_irq();
}
