#include "icons.h"
#include "get_next_line.h"

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
		return (1);

	fd = open(list_path, O_READ);
	free(list_path);
	log("%i\n", fd);
	if (fd < 0)
		return (1);

	// count valid icons
	while ((line = get_next_line(fd)))
	{
		if (valid_icon_path(line))
			max_icons++;

		free(line);
	}
	close(fd);
	library = malloc(sizeof(t_icon) * max_icons);
	if (!library)
		return (1);


	fd = open(path_add(path, "icons.lst"), O_READ);
	if (fd < 0)
	{
		free_library();
		return (1);
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
			return (1);
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
			cleanup_splitted(parts);
			free(line);
			close(fd);
			free_library();
			return (1);
		}

		if (!bmp_load_image(&library[library_count].texture, full_path))
		{
			free(full_path);
			cleanup_splitted(parts);
			free(line);
			close(fd);
			free_library();
			return (1);
		}
		library_count++;
		free(full_path);
		cleanup_splitted(parts);
		free(line);
	}
	close(fd);
	return (0);
}

BmpTexture	*get_icon(char *name)
{
    for (size_t i = 0; i < library_count; i++)
    {
        if (!ft_strcmp(library[i].name, name))
            return &library[i].texture;
    }
    return 0;
}