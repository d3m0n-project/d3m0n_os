#include "proc/events.h"

static void fifo_init(event_fifo_t *q)
{
	q->head = 0;
	q->tail = 0;
	q->count = 0;
}

static bool fifo_is_empty(const event_fifo_t *q)
{
	return q->count == 0;
}

static bool fifo_is_full(const event_fifo_t *q)
{
	return q->count == FIFO_CAPACITY;
}

static bool fifo_push(event_fifo_t *q, const event_t *event)
{
	if (fifo_is_full(q))
		return false;

	q->buffer[q->tail] = *event;

	q->tail = (q->tail + 1) % FIFO_CAPACITY;
	q->count++;

	return true;
}

static bool fifo_pop(event_fifo_t *q, event_t *out)
{
	if (fifo_is_empty(q))
		return false;

	*out = q->buffer[q->head];

	q->head = (q->head + 1) % FIFO_CAPACITY;
	q->count--;

	return true;
}