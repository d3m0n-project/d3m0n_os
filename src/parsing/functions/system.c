#include "scripting.h"
#include "stats.h"
#include "libft.h"
#include "time.h"

#define UPDATE_DELTA_TIME_MS	500

char	*fn_system_var_get(void **args)
{
	char		*key = ((char **)args)[0];
	uint64_t	current_time = time_us() / 1000;
	
	for (int i=0; i<STATS_COUNT; i++)
	{
		if (!ft_strcmp(key, (char *)stats[i].name))
		{
			if (current_time - stats[i].last_update > UPDATE_DELTA_TIME_MS)
				get_stat(i, 1); // update stat

			char *int_value = ft_itoa(stats[i].value);
			if (!int_value)
				return 0;

			if (stats[i].unit)
			{
				char *final = ft_strjoin(int_value, stats[i].unit);
				free(int_value);
				return final;
			}
			else
			{
				if (stats[i].min == 0 && stats[i].max == 1) // bool
				{
					char	*final = ft_strdup((stats[i].value)?"true":"false");
					free(int_value);
					return final;
				}
			}
			return int_value;
		}
	}
	return 0;
}