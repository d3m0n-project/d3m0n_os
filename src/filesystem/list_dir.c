#include "filesystem.h"
#include "memory.h"
#include "fat32.h"
#include "libft.h"

typedef struct s_file_list
{
	char	**files;
	size_t  count;
	size_t  capacity;
}	t_file_list;

static t_file_list	*g_file_list;

static void	collect_file(const char *name, uint32_t size)
{
	(void)size;

	if (g_file_list->count == g_file_list->capacity)
	{
		size_t new_capacity = g_file_list->capacity ? g_file_list->capacity * 2 : 8;

		char **new_files = malloc(sizeof(char *) * (new_capacity + 1));
		if (!new_files)
			return;

		if (g_file_list->files)
		{
			ft_memcpy(new_files, g_file_list->files, sizeof(char *) * g_file_list->count);
			free(g_file_list->files);
		}

		g_file_list->files = new_files;
		g_file_list->capacity = new_capacity;
	}

	g_file_list->files[g_file_list->count] = ft_strdup(name);
	if (!g_file_list->files[g_file_list->count])
		return;

	g_file_list->count++;
	g_file_list->files[g_file_list->count] = 0;
}

char	**list_dir_files(const char *path)
{
	t_file_list list;

	list.files = 0;
	list.count = 0;
	list.capacity = 0;

	g_file_list = &list;

	fat32_list_dir(path, collect_file);

	g_file_list = 0;

	return list.files;
}

static void	print_file(const char *name, uint32_t size)
{
	log("    %s (%u bytes)\n", LOG_NONE, name, size);
}
void	list_dir(const char *path)
{
	log("File listing in %s\n", LOG_NONE, path ? path : "/");
	fat32_list_dir(path, print_file);
}