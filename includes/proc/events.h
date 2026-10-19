#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

#define PROC_MAX_EVENTS

typedef struct {
	uint32_t		type;
	uint64_t		timestamp;
	int32_t			x;
	int32_t			y;
}	event_t;

typedef struct {
	event_t		buffer[FIFO_CAPACITY];
	size_t		head;
	size_t		tail;
	size_t		count;
}	event_fifo_t;

#endif