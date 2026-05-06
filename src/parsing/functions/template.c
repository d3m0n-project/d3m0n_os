#include "scripting.h"
#include "log.h"
#include "filesystem.h"

void	template_load(void **args)
{
	char	*path = ((char **)args)[0];
	int x = *(int *)(args)[1];
	int y = *(int *)(args)[2];

	char	**strings = (char **)&(args[3]);
	int fd = open(path, O_READ);
	if (fd < 0)
	{
		log("Could not find template file: '%s'\n", LOG_ERROR | LOG_INDENT, path);
		return;
	}

	int i = 0;
	while (strings[i])
	{
		char	*arg = strings[i];
		int j = 0;
		while (arg[j])
		{
			if (arg[j] == '\n' || arg[j] == '\r' || arg[j] == '"' || arg[j] == '\\')
			{
				log("Invalid template replacement string for $%i: '%s'\n", LOG_ERROR | LOG_INDENT, i, arg);
				return;
			}
			j++;
		}
		log("'%s'\n", LOG_INDENT | LOG_ERROR, arg);
		i++;
	}

	log("loading '%s' at x:%i, y:%i\n", LOG_ERROR, path, x, y);
	//(path, x, y, "string 1", "string 2", ...)
}