#include "icons.h"
#include "get_next_line.h"
#include "settings.h"
#include "libft.h"

typedef struct	s_icon
{
	BmpTexture	texture;
	char		name[32];
}	t_icon;

static t_icon	*library = 0;
static size_t	library_count = 0;

static int	valid_icon_path(const char *path)
{
	char	**parts;
	int		ok = 0;

	parts = ft_split(path, '/');
	if (!parts)
		return (0);

	if (parts[0] && parts[1] && !parts[2])
	{
		size_t len = ft_strlen(parts[1]);

		if (len > 4 && !ft_strcmp(parts[1] + len - 4, ".bmp"))
			ok = 1;
	}

	cleanup_splitted(parts);
	return (ok);
}


static void	free_library(void)
{
	if (!library)
		return;

	for (size_t i = 0; i < library_count; i++)
		free_bmp_texture(&library[i].texture);

	free(library);
	library = 0;
	library_count = 0;
}

int	load_icon_pack(char *path)
{
	char	*list_path;
	int		fd;
	char	*line;
	size_t	max_icons = 0;

	free_library();

	list_path = ft_strjoin(path, "/icons.lst");
	if (!list_path)
		return 1;

	fd = open(list_path, O_READ);
	if (fd < 0)
	{
		free(list_path);
		log("ICONS: Could not find icons.lst in icon pack\n", LOG_ERROR);
		return 1;
	}

	// count valid icons
	while ((line = get_next_line(fd)))
	{
		size_t i = ft_strlen(line);

		while (i > 0 && (line[i - 1] == '\n' || line[i - 1] == '\r'))
		{
			line[i - 1] = '\0';
			i--;
		}
		if (!valid_icon_path(line))
		{
			log("ICONS: Invalid icon path at l.%i: '%s'\n", LOG_ERROR, max_icons+1, line);
			free(line);
			close(fd);
			free(list_path);
			return 1;
		}	
		max_icons++;
		free(line);
	}
	close(fd);
	log("Found %lu icons in the icon pack!\n", LOG_SUCCESS, max_icons);
	library = malloc(sizeof(t_icon) * max_icons);
	if (!library)
	{
		free(list_path);
		log("ICONS: Could not allocate library\n", LOG_ERROR);
		return 1;
	}


	fd = open(list_path, O_READ);
	free(list_path);
	if (fd < 0)
	{
		
		log("ICONS: Could not find icons.lst in icon pack\n", LOG_ERROR);
		free_library();
		return 1;
	}
	while ((line = get_next_line(fd)))
	{
		char	**parts;
		char	*full_path;
		size_t	len;

		len = ft_strlen(line);
		if (len && line[len - 1] == '\n')
			line[len - 1] = 0;
		if (!valid_icon_path(line))
		{
			free(line);
			continue;
		}


		parts = ft_split(line, '/');
		if (!parts)
		{
			free(line);
			close(fd);
			free_library();
			log("ICONS: Could not allocate path folder parts\n", LOG_ERROR);
			return 1;
		}
		len = ft_strlen(parts[1]);
		parts[1][len - 4] = 0;


		/* category.iconname */
		ft_strlcpy(library[library_count].name, parts[0], sizeof(library[library_count].name));
		ft_strlcat(library[library_count].name, ".", sizeof(library[library_count].name));
		ft_strlcat(library[library_count].name, parts[1], sizeof(library[library_count].name));


		full_path = path_add(path, line);
		if (!full_path)
		{
			log("ICONS: Could not allocate full path of icon\n", LOG_ERROR);
			cleanup_splitted(parts);
			free(line);
			close(fd);
			free_library();
			return 1;
		}

		if (bmp_load_image(&library[library_count].texture, full_path))
		{
			log("ICONS: Could not load image at %s\n", LOG_ERROR, full_path);
			free(full_path);
			cleanup_splitted(parts);
			free(line);
			close(fd);
			free_library();
			return 1;
		}
		library_count++;
		free(full_path);
		cleanup_splitted(parts);
		free(line);
	}
	close(fd);
	return (0);
}

BmpTexture	*get_icon(char *name, t_conf *conf)
{
	int		setting_index = -1;

	if (conf)
	{
		// check if that is an alias in the config
		for (setting_index = 0; setting_index < CONF_FIELD_COUNT; setting_index++)
		{
			if (conf_offsets[setting_index].type == TYPE_STRING && !ft_strcmp((char *)conf_offsets[setting_index].key, name))
			{
				name = (char *)conf + conf_offsets[setting_index].offset;
				break;
			}
		}
	}
	

	// search it
	for (size_t i = 0; i < library_count; i++)
	{
		if (!ft_strcmp(library[i].name, name))
			return &library[i].texture;
	}
	log("ICONS: Could not find icon '%s'\n", LOG_ERROR, name);
	return 0;
}