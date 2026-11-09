#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

#define PROC_MAX_EVENTS	128
#define APP_KEY_ARROW_UP	0x100
#define APP_KEY_ARROW_DOWN	0x101

typedef enum: uint32_t
{
	EVENT_UNKNOWN=0,
	EVENT_MOUSECLICK1=1,
	EVENT_MOUSECLICK2=2,
	EVENT_MOUSECLICK3=3,
	EVENT_MOUSECLICK4=4,
	EVENT_KEYPRESS=5,
	EVENT_KEYRELEASE=6,
	EVENT_TERMINAL_OUTPUT=7,
	EVENT_TERMINAL_EXIT=8,
}	e_event_type;

#define EVENT_IS_MOUSE(x)		(x >= EVENT_MOUSECLICK1 && x <= EVENT_MOUSECLICK4)
#define EVENT_IS_KEYBOARD(x)	((x) == EVENT_KEYPRESS || (x) == EVENT_KEYRELEASE)

typedef struct {
	e_event_type	type;
	uint64_t		timestamp;
	int32_t			x;
	int32_t			y;
	int32_t			key;
	uint32_t		modifiers;
}	event_t;

typedef struct {
	event_t			buffer[PROC_MAX_EVENTS];
	size_t			head;
	size_t			tail;
	size_t			count;
}	event_fifo_t;

#endif
