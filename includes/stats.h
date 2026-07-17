#ifndef STATS_H
#define STATS_H

#include "log.h"

// battery
int		is_charging(void);
int		get_battery_level(void);

// storage
int		get_read_speed(void);


#define STAT_ENTRY(name, min, max, unit, update_fn)			(t_stat_entry){#name, min, min, max, unit, 0, update_fn},
#define STAT_ENUM_COUNTER(name, min, max, unit, update_fn)	STATIDX_##name,

#define STATISTICS(X) \
	X(battery_charging,		0,	1,			0,			is_charging) \
	X(battery_percentage,	0,	100,		"%",		get_battery_level) \
	X(read_speed,			0,	1000,	" Mb/s",		get_read_speed)

typedef struct s_stat_entry
{
	const char	*name;
	int			value;
	int			min;
	int			max;
	const char	*unit;
	uint64_t	last_update;
	int			(*update)(void);
}	t_stat_entry;

enum {
    STATS_COUNT =
	#define COUNT(...) +1
		0 STATISTICS(COUNT)
	#undef COUNT
};


typedef enum {
	STATISTICS(STAT_ENUM_COUNTER)
}	e_stat_idx;

extern t_stat_entry	stats[STATS_COUNT];


int		get_stat(e_stat_idx idx, int update);

#endif