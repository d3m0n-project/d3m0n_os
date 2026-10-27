#include "proc/fifo.h"
#include "stdlib.h"

void	fifo_init(event_fifo_t *q)
{
	q->head = 0;
	q->tail = 0;
	q->count = 0;
}

bool	fifo_is_empty(const event_fifo_t *q)
{
	return q->count == 0;
}

bool	fifo_is_full(const event_fifo_t *q)
{
	return q->count == PROC_MAX_EVENTS;
}

bool	fifo_push(event_fifo_t *q, const event_t *event)
{
	if (fifo_is_full(q))
		return false;

	q->buffer[q->tail] = *event;

	q->tail = (q->tail + 1) % PROC_MAX_EVENTS;
	q->count++;

	return true;
}

bool	fifo_pop(event_fifo_t *q, event_t *out)
{
	if (fifo_is_empty(q))
		return false;

	*out = q->buffer[q->head];

	q->head = (q->head + 1) % PROC_MAX_EVENTS;
	q->count--;

	return true;
}