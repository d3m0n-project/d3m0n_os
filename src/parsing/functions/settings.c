#include "scripting.h"
#include "settings.h"
#include "log.h"
#include "libft.h"

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
	int		not_found = 0;
	char	*result = get_setting("/config", key, &not_found);
	if (!result && not_found)
		return 0;
	return result;
}