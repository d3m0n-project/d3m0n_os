#include "stats.h"
#include "time.h"

t_stat_entry	stats[STATS_COUNT] = {STATISTICS(STAT_ENTRY)};

int	get_stat(e_stat_idx idx, int update)
{
	if (update)
	{
		int value = stats[idx].update();
		if (value < stats[idx].min || value > stats[idx].max) 
		{
			log("STAT: Stat '%s' value is wrong, min=%i max=%i, found %i\n", LOG_ERROR, stats[idx].name, stats[idx].min, stats[idx].max, value);
			return 0;
		}
		stats[idx].value = value;
		stats[idx].last_update = time_us() / 1000;
	}
	return stats[idx].value;
}