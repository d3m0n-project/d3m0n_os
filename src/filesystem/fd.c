#include "filesystem.h"
#include "fat32.h"
#include "get_next_line.h"

typedef struct fs_fd
{
	FAT32_File		file;
	file_open_mode	mode;
}	fs_fd;


static fs_fd	g_fds[FS_MAX_FDS] = {0};

static int	is_valid_open_flags(int flags)
{
	int access_mode;

	if (flags & ~(O_READ | O_WRITE | O_CREATE | O_APPEND | O_TRUNC))
		return (0);
	if ((flags & O_APPEND) && !(flags & O_WRITE))
		return (0);
	access_mode = flags & (O_READ | O_WRITE);
	if (access_mode == 0)
		return (0);
	return (1);
}

int		open(const char *path, int flags)
{
	int			fd;
	int			i;
	FAT32_File	file;

	if (!path || !is_valid_open_flags(flags))
		return (-1);
	fd = -1;
	for (i = 0; i < FS_MAX_FDS; i++)
	{
		if (g_fds[i].mode == FILE_NOT_CREATED)
		{
			fd = i;
			break;
		}
	}
	if (fd == -1)
		return (-1);
	file = fat32_open(path);
	if (file.first_cluster == 0)
	{
		if (flags & O_CREATE)
			file = fat32_create(path);
		if (file.first_cluster == 0)
			return (-1);
	}
	if (file.is_dir)
		return (-1);
	g_fds[fd].file = file;
	g_fds[fd].mode = (file_open_mode)(flags & (O_READ | O_WRITE | O_CREATE | O_APPEND));
		if ((flags & O_TRUNC) && (flags & O_WRITE))
		{
			fat32_delete(path);
			file = fat32_create(path);
			if (file.first_cluster == 0)
				return (-1);
			g_fds[fd].file = file;
		}
	if (g_fds[fd].mode & O_APPEND)
		g_fds[fd].file.pos = g_fds[fd].file.size;
	return (fd);
}

int		file_exists(const char *path)
{
	FAT32_File	file;

	if (!path)
		return (0);
	file = fat32_open(path);
	return (file.first_cluster != 0 && !file.is_dir);
}

int		dir_exists(const char *path)
{
	FAT32_File	file;

	if (!path)
		return (0);
	file = fat32_open(path);
	return (file.first_cluster != 0 && file.is_dir);
}

// TODO: validate paths
char	*path_add(char *path, char *path2)
{
	size_t	l1 = ft_strlen(path);
	size_t	l2 = ft_strlen(path2);
	char	*output = ft_calloc(l1 + l2 + 1, sizeof(char));
	if (!output)
		return 0;
	
	ft_strlcpy(output, path, l1 + l2 + 1);
	ft_strlcat(output, path2, l1 + l2 + 1);
	return output;
}

int		file_delete(const char *path)
{
	if (!path)
		return 1;
	if (!file_exists(path) || fat32_delete(path))
		return 1;
	return 0;
}

uint32_t	read(int fd, char *buffer, uint32_t count)
{
	if (fd >= FS_MAX_FDS || fd < 0 || g_fds[fd].mode == FILE_NOT_CREATED)
		return (-1);
	if (!(g_fds[fd].mode & O_READ))
		return (-1);
	
	return (fat32_read(&(g_fds[fd].file), (uint8_t *)buffer, count));
}


int		write(int fd, const char *buffer, uint32_t count)
{
	if (fd >= FS_MAX_FDS || fd < 0 || g_fds[fd].mode == FILE_NOT_CREATED)
		return (-1);

	if (!(g_fds[fd].mode & O_WRITE)) // not in write mode
		return (-1);
	if (g_fds[fd].mode & O_APPEND)
		g_fds[fd].file.pos = g_fds[fd].file.size;
	
	return (fat32_write(&(g_fds[fd].file), (const uint8_t *)buffer, count));
}


int		close(int fd)
{
	if (fd >= FS_MAX_FDS || fd < 0 || g_fds[fd].mode == FILE_NOT_CREATED)
		return (-1);

	g_fds[fd].mode = FILE_NOT_CREATED;
	fat32_close(&(g_fds[fd].file));
	clear_fd_buffer(fd); // used for GNL buffer clearing
	return 0;
}

uint32_t	lseek(int fd, int32_t offset, e_seek_directive whence)
{
	if (fd < 0 || fd >= FS_MAX_FDS || g_fds[fd].mode == FILE_NOT_CREATED)
		return -1;

	int32_t new_pos;

	switch (whence)
	{
		case SEEK_SET:
			new_pos = offset;
			break;
		case SEEK_CUR:
			new_pos = g_fds[fd].file.pos + offset;
			break;
		case SEEK_END:
			new_pos = g_fds[fd].file.size + offset;
			break;
		default:
			return -1;
	}

	if (new_pos < 0)
		return -1;

	if (new_pos > (int32_t)g_fds[fd].file.size)
		new_pos = g_fds[fd].file.size;

	g_fds[fd].file.pos = new_pos;

	return new_pos;
}
