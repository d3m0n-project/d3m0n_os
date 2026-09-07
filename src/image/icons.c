#include "image/icons.h"
#include "filesystem/get_next_line.h"
#include "settings.h"
#include "libft.h"
#include "../../compiler/sdk/lib/app/svg.h"
#include "display/display.h"

typedef struct	s_icon
{
	BmpTexture		bmp_texture;
	t_svg			svg_texture;

	e_icon_format	format;
	char			name[32];
}	t_icon;

static t_icon	*library = 0;
static size_t	library_count = 0;

#define DEFAULT_ICON_PATH	"/themes/default.bmp"

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

		if (len > 4 && (!ft_strcmp(parts[1] + len - 4, ".bmp") || !ft_strcmp(parts[1] + len - 4, ".BMP")))
			ok = 1;
		if (len > 4 && (!ft_strcmp(parts[1] + len - 4, ".svg") || !ft_strcmp(parts[1] + len - 4, ".SVG")))
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
		if (library[i].format == ICON_FORMAT_BMP && library[i].bmp_texture.pixels)
			free_bmp_texture(&library[i].bmp_texture);
		if (library[i].format == ICON_FORMAT_SVG && library[i].svg_texture.data)
			kfree(&library[i].svg_texture.data);
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
	list_path = ft_strjoin(path, "/icons.lst");
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
		size_t i = ft_strlen(line);
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
	library = kmalloc(sizeof(t_icon) * (max_icons + 1));
	if (!library)
	{
		kfree(list_path);
		log("ICONS: Could not allocate library\n", LOG_ERROR);
		return 1;
	}

	// load default icon
	library[library_count++] = (t_icon){0};
	ft_strlcpy(library[0].name, "default", 32);
	bmp_load_image(&library[0].bmp_texture, DEFAULT_ICON_PATH); // TODO: make default dynamic


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

		len = ft_strlen(line);
		if (len > 1 && line[len - 1] == '\n')
		{
			line[len - 1] = 0;
			if (line[len - 2] == '\r')
				line[len - 2] = 0;
		}
			
		if (!valid_icon_path(line))
		{
			kfree(line);
			continue;
		}


		parts = ft_split(line, '/');
		if (!parts)
		{
			kfree(line);
			close(fd);
			free_library();
			log("ICONS: Could not allocate path folder parts\n", LOG_ERROR);
			return 1;
		}
		len = ft_strlen(parts[1]);
		parts[1][len - 4] = 0;


		// category.iconname
		ft_strlcpy(library[library_count].name, parts[0], sizeof(library[library_count].name));
		ft_strlcat(library[library_count].name, ".", sizeof(library[library_count].name));
		ft_strlcat(library[library_count].name, parts[1], sizeof(library[library_count].name));


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

		char *format = ft_strrchr(full_path, '.');
		if (!ft_strcmp(format, ".svg") || !ft_strcmp(format, ".SVG"))
			library[library_count].format = ICON_FORMAT_SVG;
		else
			library[library_count].format = ICON_FORMAT_BMP;

		if (
			(library[library_count].format == ICON_FORMAT_BMP && bmp_load_image(&library[library_count].bmp_texture, full_path))
			|| (library[library_count].format == ICON_FORMAT_SVG && svg_read_file(&library[library_count].svg_texture, full_path))
		)
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

void	*get_icon(char *name, t_conf *conf, e_icon_format *type)
{
	int		setting_index = -1;
	*type = ICON_FORMAT_UNKNOWN;
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
		{
			*type = library[i].format;
			if (library[i].format == ICON_FORMAT_BMP)
				return &library[i].bmp_texture;
			if (library[i].format == ICON_FORMAT_SVG)
				return &library[i].svg_texture;
			return 0;
		}
	}
	log("ICONS: Could not find icon '%s'\n", LOG_ERROR, name);
	return 0;
}

void		draw_icon(int x, int y, int w, int h, const char *icon_name, uint32_t override_color)
{
	t_conf	*conf = get_config();
	e_icon_format type = ICON_FORMAT_UNKNOWN;
	void	*icon = get_icon((char *)icon_name, conf, &type);
	if (!icon || type == ICON_FORMAT_UNKNOWN)
	{
		log("DRAW ICON: Unknown icon type for %s\n", LOG_WARNING | LOG_INDENT, icon_name);
		return;
	}
	if (type == ICON_FORMAT_SVG)
		draw_svg_buff(x, y, w, h, ((t_svg *)icon)->data, ((t_svg *)icon)->size + 1, 0);//override_color);
	else if (type == ICON_FORMAT_BMP)
		draw_bmp(x, y, w, h, icon, override_color);
}