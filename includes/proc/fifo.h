#ifndef FIFO_H
#define FIFO_H

#include "types.h"

#include "../../compiler/sdk/lib/app/events.h"

void	fifo_init(event_fifo_t *q);
bool	fifo_is_empty(const event_fifo_t *q);
bool	fifo_is_full(const event_fifo_t *q);
bool	fifo_push(event_fifo_t *q, const event_t *event);
bool	fifo_pop(event_fifo_t *q, event_t *out);

#endif