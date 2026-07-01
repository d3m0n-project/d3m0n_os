#include "package_manager.h"
#include "filesystem.h"
#include "memory.h"
#include "scripting.h"
#include "libft.h"

static	void	cleanup_splitted(char **splitted)
{
	if (!splitted)
		return;
	int i = 0;
	while (splitted[i])
	{
		free(splitted[i]);
		i++;
	}
	free(splitted);
}

// format:
//	 com.provider.module_name.path.to => /apps/module_name/path/to/
char *get_app_path_from_package(char *package, e_package_request_type type)
{
	char	*out;
	char	**parts;
	const char *suffix;
	size_t  len;
	int	 count;

	parts = ft_split(package, '.');
	if (!parts)
		return (0);

	suffix = 0;
	switch (type)
	{
		case PACKAGE_PATH:		   suffix = "source/"; break;
		case PACKAGE_MANIFEST:	   suffix = "source/app"; break;
		case PACKAGE_MAIN_LAYOUT:  suffix = "source/layouts/main.layout"; break;
		case PACKAGE_MAIN_SOURCE:  suffix = "source/src/main.src"; break;
		case PACKAGE_LAYOUT_PATH:  suffix = "source/layouts/"; break;
		case PACKAGE_SOURCE_PATH:  suffix = "source/src/"; break;
		default:
			cleanup_splitted(parts);
			return (0);
	}

	count = 0;
	while (parts[count])
		count++;

	len = ft_strlen("/apps/");

	for (int i = 2; i < count; i++)
		len += ft_strlen(parts[i]) + 1;

	len += ft_strlen(suffix);

	out = ft_calloc(len + 1, 1);
	if (!out)
	{
		cleanup_splitted(parts);
		return (0);
	}

	ft_strlcpy(out, "/apps/", len + 1);

	for (int i = 2; i < count; i++)
	{
		ft_strlcat(out, parts[i], len + 1);
		ft_strlcat(out, "/", len + 1);
	}

	ft_strlcat(out, suffix, len + 1);

	cleanup_splitted(parts);
	return (out);
}

int	load_app_list(void)
{
	char	**apps_dir = list_dir_files("/apps");
	if (!apps_dir)
	{
		log("APP LIST: Could not find application directory\n", LOG_ERROR);
		return 1;
	}
	for (int i = 0; apps_dir[i]; i++)
	{
		if (apps_dir[i][0] != '.')
		{
			log("Found app: '%s'\n", 0, apps_dir[i]);
		}
		free(apps_dir[i]);
	}
	free(apps_dir);
	//"Hello World", "default", "com.4re5.d3m0n.test.helloWorld"
	return 0;
}