#include "scripting.h"
#include "settings.h"
#include "log.h"
#include "libft.h"

#include "time.h"

char*	fn_settings_set(void **args)
{
	char	*key = ((char **)args)[0];
	char	*value = ((char **)args)[1];
	
	if (set_setting("/config", key, value))
		log("SETTINGS.SET: Failed to set key='%s' value='%s'\n", LOG_ERROR | LOG_INDENT, key, value);
	return 0;
}

char*	fn_settings_get(void **args)
{
	char	*key = ((char **)args)[0];
	t_conf	*conf = get_config();

	for (int i=0; i<CONF_FIELD_COUNT; i++)
	{
		if (!ft_strcmp((char *)conf_offsets[i].key, key))
		{
			uintptr_t offset = conf_offsets[i].offset + (uintptr_t)conf;
			if (conf_offsets[i].type == TYPE_INT)
				return ft_itoa(*(int *)offset);
			else if (conf_offsets[i].type == TYPE_STRING)
				return ft_strdup((char *)offset);
			else
				log("SETTINGS.GET: Invalid offset type!\n", LOG_WARNING);
		}
	}
	log("SETTINGS.GET: Failed to get key='%s'\n", LOG_ERROR | LOG_INDENT, key);
	return 0;
}