#include "image/icons.h"
#include "filesystem/get_next_line.h"
#include "settings.h"
#include "stdlib.h"
#include "../../compiler/sdk/lib/app/parsing/svg.h"
#include "display/display.h"

typedef struct	s_icon_elem
{
	t_svg			svg_texture;

	e_icon_format	format;
	char			name[32];
}	t_icon_elem;

static t_icon_elem	*library = 0;
static size_t	library_count = 0;

static int	valid_icon_path(const char *path)
{
	char	**parts;
	int		ok = 0;

	parts = split(path, '/');
	if (!parts)
		return (0);

	if (parts[0] && parts[1] && !parts[2])
	{
		size_t len = strlen(parts[1]);

		if (len > 4 && (!strcmp(parts[1] + len - 4, ".svg") || !strcmp(parts[1] + len - 4, ".SVG")))
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
	{
		if (library[i].format == ICON_FORMAT_SVG && library[i].svg_texture.data)
			kfree(library[i].svg_texture.data);
	}

	kfree(library);
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
	list_path = strjoin(path, "/icons.lst");
	if (!list_path)
		return 1;

	fd = open(list_path, O_READ);
	if (fd < 0)
	{
		kfree(list_path);
		log("ICONS: Could not find icons.lst in icon pack\n", LOG_ERROR);
		return 1;
	}

	// count valid icons
	while ((line = get_next_line(fd)))
	{
		size_t i = strlen(line);
		while (i > 0 && (line[i - 1] == '\n' || line[i - 1] == '\r'))
		{
			line[i - 1] = '\0';
			i--;
		}
		if (!valid_icon_path(line))
		{
			log("ICONS: Invalid icon path at l.%i: '%s'\n", LOG_ERROR, max_icons+1, line);
			kfree(line);
			close(fd);
			kfree(list_path);
			return 1;
		}	
		max_icons++;
		kfree(line);
	}
	close(fd);
	log("Found %lu icons in the icon pack!\n", LOG_SUCCESS, max_icons);
	if (max_icons == 0)
	{
		kfree(list_path);
		return 0;
	}
	library = kmalloc(sizeof(t_icon_elem) * max_icons);
	if (!library)
	{
		kfree(list_path);
		log("ICONS: Could not allocate library\n", LOG_ERROR);
		return 1;
	}

	fd = open(list_path, O_READ);
	kfree(list_path);
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

		len = strlen(line);
		while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
		{
			line[--len] = 0;
		}
			
		if (!valid_icon_path(line))
		{
			kfree(line);
			continue;
		}


		parts = split(line, '/');
		if (!parts)
		{
			kfree(line);
			close(fd);
			free_library();
			log("ICONS: Could not allocate path folder parts\n", LOG_ERROR);
			return 1;
		}
		len = strlen(parts[1]);
		parts[1][len - 4] = 0;


		// category.iconname
		strlcpy(library[library_count].name, parts[0], sizeof(library[library_count].name));
		strlcat(library[library_count].name, ".", sizeof(library[library_count].name));
		strlcat(library[library_count].name, parts[1], sizeof(library[library_count].name));

		full_path = path_add(path, line);
		if (!full_path)
		{
			log("ICONS: Could not allocate full path of icon\n", LOG_ERROR);
			cleanup_splitted(parts);
			kfree(line);
			close(fd);
			free_library();
			return 1;
		}

		library[library_count].format = ICON_FORMAT_SVG;

		if (svg_read_file(&library[library_count].svg_texture, full_path))
		{
			log("ICONS: Could not load image at %s\n", LOG_ERROR, full_path);
			kfree(full_path);
			cleanup_splitted(parts);
			kfree(line);
			close(fd);
			free_library();
			return 1;
		}
		library_count++;
		kfree(full_path);
		cleanup_splitted(parts);
		kfree(line);
	}
	close(fd);
	return (0);
}

void	*get_icon(const char *name, t_conf *conf, e_icon_format *type)
{
	int		setting_index = -1;
	const char *lookup_name = name;
	if (!name || !type)
		return 0;
	*type = ICON_FORMAT_UNKNOWN;
	if (conf)
	{
		// check if that is an alias in the config
		for (setting_index = 0; setting_index < CONF_FIELD_COUNT; setting_index++)
		{
			if (conf_offsets[setting_index].type == TYPE_STRING && !strcmp((char *)conf_offsets[setting_index].key, name))
			{
				lookup_name = (char *)conf + conf_offsets[setting_index].offset;
				break;
			}
		}
	}
	

	// search it
	for (size_t i = 0; i < library_count; i++)
	{
		if (!strcmp(library[i].name, lookup_name))
		{
			*type = library[i].format;
			if (library[i].format == ICON_FORMAT_SVG)
				return &library[i].svg_texture;
			return 0;
		}
	}
	log("ICONS: Could not find icon '%s'\n", LOG_ERROR, lookup_name);
	return 0;
}

void		draw_icon(int x, int y, int w, int h, const char *icon_name, uint32_t override_color)
{
	t_conf	*conf = get_config();
	e_icon_format type = ICON_FORMAT_UNKNOWN;
	void	*icon = get_icon(icon_name, conf, &type);
	if (!icon || type == ICON_FORMAT_UNKNOWN)
	{
		log("DRAW ICON: Unknown icon type for %s\n", LOG_WARNING | LOG_INDENT, icon_name);
		return;
	}
	if (type == ICON_FORMAT_SVG)
		draw_svg_buff(x, y, w, h, ((t_svg *)icon)->data, ((t_svg *)icon)->size, override_color);
}
