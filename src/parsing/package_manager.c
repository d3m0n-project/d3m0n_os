#include "package_manager.h"
#include "filesystem.h"
#include "memory.h"
#include "scripting.h"
#include "libft.h"
#include "get_next_line.h"
#include "settings.h"

void	cleanup_splitted(char **splitted)
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
	char **modules_list = list_dir_files("/apps");
	if (!modules_list)
	{
		log("APP LIST: Could not find application directory\n", LOG_ERROR);
		return 1;
	}

	if (apps)
	{
		for (int i = 0; apps[i].name; i++)
		{
			free(apps[i].name);
			free(apps[i].icon);
			free(apps[i].package);
		}
		free(apps);
		apps = 0;
	}

	int total_n_apps = 0;
	// count apps
	for (int i = 0; modules_list[i]; i++)
	{
		if (modules_list[i][0] == '.')
			continue;
		char	*module_path = path_add("/apps/", modules_list[i]);
		if (!module_path)
			continue;		

		char *package_lst = path_add(module_path, "/packages.lst");
		if (!package_lst)
		{
			free(module_path);
			continue;
		}

		int fd = open(package_lst, O_READ);
		if (fd < 0)
		{
			log("Could not open package.lst for module %s\n", LOG_ERROR | LOG_INDENT, package_lst);
			free(module_path);
			continue;
		}
		free(package_lst);

		char *line;
		while ((line = get_next_line(fd)))
		{
			if (line[0] != '#' && line[0] != '\n' && line[0] != '\0')
				total_n_apps++;
			free(line);
		}
		close(fd);
	}
	log(" --- Found %i total apps ---\n", LOG_INFO, total_n_apps);

	apps = ft_calloc(total_n_apps + 1, sizeof(t_app));
	if (!apps)
	{
		cleanup_splitted(modules_list);
		return 1;
	}

	// load apps
	int app_index = 0;
	for (int i = 0; modules_list[i]; i++)
	{
		if (modules_list[i][0] == '.')
			continue;

		char	*module_path = path_add("/apps/", modules_list[i]);
		if (!module_path)
			continue;		

		char *package_lst = path_add(module_path, "/packages.lst");
		if (!package_lst)
		{
			free(module_path);
			continue;
		}

		int fd = open(package_lst, O_READ);
		if (fd < 0)
		{
			log("Could not open package.lst for module %s\n", LOG_ERROR | LOG_INDENT, package_lst);
			free(module_path);
			continue;
		}
		free(package_lst);

		char *line;
		while ((line = get_next_line(fd)))
		{
			if (line[0] == '#' || line[0] == '\n' || line[0] == '\0')
			{
				free(line);
				continue;
			}

			line[ft_strcspn(line, "\r\n")] = '\0';

			// /apps/module/<relative path>
			char *manifest = path_add(module_path, line);

			int mfd = open(manifest, O_READ);
			if (mfd >= 0)
			{
				char *mline;
				while ((mline = get_next_line(mfd)))
				{
					mline[ft_strcspn(mline, "\r\n")] = '\0';

					if (!ft_strncmp(mline, "name: ", 6))
						apps[app_index].name = ft_strdup(mline + 6);
					else if (!ft_strncmp(mline, "icon: ", 6))
						apps[app_index].icon = ft_strdup(mline + 6);
					else if (!ft_strncmp(mline, "package: ", 9))
						apps[app_index].package = ft_strdup(mline + 9); // TODO: build package from provider and path
					free(mline);
				}
				close(mfd);
			} else
				log("Could not find manifest at %s\n", LOG_ERROR | LOG_INDENT, manifest);
			free(manifest);
			app_index++;
			free(line);
		}
		free(module_path);
		close(fd);
	}

	cleanup_splitted(modules_list);
	return 0;
}