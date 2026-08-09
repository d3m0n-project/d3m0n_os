#ifndef MUTEX_H
#define MUTEX_H

#include "types.h"

typedef struct s_mutex
{
	volatile uint32_t	locked;
	uint32_t			irq_state;
}	t_mutex;

#define MUTEX_INITIALIZER	{ 0, 0 }

void	mutex_init(t_mutex *mutex);
void	mutex_lock(t_mutex *mutex);
void	mutex_unlock(t_mutex *mutex);

#endif
