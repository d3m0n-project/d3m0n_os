#ifndef CONFIG_QUERY_H
#define CONFIG_QUERY_H

#include "types.h"

#define CONFIG_QUERY_STRING_SIZE 128
#define CONFIG_QUERY_STRING 0
#define CONFIG_QUERY_INTEGER 1

typedef struct s_config_value
{
	uint32_t type;
	union
	{
		char string[CONFIG_QUERY_STRING_SIZE];
		int32_t integer;
	} value;
} t_config_value;

#endif
