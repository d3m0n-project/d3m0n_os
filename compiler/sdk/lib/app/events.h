#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

#define PROC_MAX_EVENTS	128

typedef enum: uint32_t
{
	EVENT_UNKNOWN=0,
	EVENT_MOUSECLICK1=1,
	EVENT_MOUSECLICK2=2,
	EVENT_MOUSECLICK3=3,
	EVENT_MOUSECLICK4=4,
}	e_event_type;

#define EVENT_IS_MOUSE(x)	(x >= EVENT_MOUSECLICK1 && x <= EVENT_MOUSECLICK4)

typedef struct {
	e_event_type	type;
	uint64_t		timestamp;
	int32_t			x;
	int32_t			y;
}	event_t;

typedef struct {
	event_t			buffer[PROC_MAX_EVENTS];
	size_t			head;
	size_t			tail;
	size_t			count;
}	event_fifo_t;

#endif