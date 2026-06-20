#include "package_manager.h"
#include "filesystem.h"
#include "memory.h"
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
//     com.provider.module_name.path.to => /apps/module_name/path/to/
char	*get_app_path_from_package(char *package, e_package_request_type type)
{
	char	*out = 0;
	char	**parts = ft_split(package, '.');

	if (!parts)
		return 0;
	size_t len = ft_strlen("/apps/") + 1;
	int count = 0;
	while (parts[count])
		count++;

	for (int i = 2; i < count; i++)
		len += ft_strlen(parts[i]) + 1;

	char	*suffix = 0;
	switch (type)
	{
	case PACKAGE_PATH:
		suffix = "source/";
		break;
	case PACKAGE_MANIFEST:
		suffix = "source/app";
		break;
	case PACKAGE_MAIN_LAYOUT:
		suffix = "source/layouts/main.layout";
		break;
	case PACKAGE_MAIN_SOURCE:
		suffix = "source/src/main.src";
		break;
	case PACKAGE_LAYOUT_PATH:
		suffix = "source/layouts/";
		break;
	case PACKAGE_SOURCE_PATH:
		suffix = "source/src/";
		break;
	default:
		cleanup_splitted(parts);
		return 0;
	}

	if (suffix)
    	len += ft_strlen(suffix);

	int new_len = len + 1;
	out = ft_calloc(new_len, sizeof(char));
	if (!out)
	{
		cleanup_splitted(parts);
		return 0;
	}
	ft_strlcpy(out, "/apps/", new_len);
	for (int i = 2; i < count; i++)
	{
		ft_strlcat(out, parts[i], new_len);
		ft_strlcat(out, "/", new_len);
	}
	if (suffix)
		ft_strlcat(out, suffix, new_len);

	cleanup_splitted(parts);
	return out;
}