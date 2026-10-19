#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

#define PROC_MAX_EVENTS	128

typedef struct {
	uint32_t		type;
	uint64_t		timestamp;
	int32_t			x;
	int32_t			y;
}	event_t;

typedef struct {
	event_t		buffer[PROC_MAX_EVENTS];
	size_t		head;
	size_t		tail;
	size_t		count;
}	event_fifo_t;

#endif