#include "filesystem/filesystem.h"
#include "filesystem/fat32.h"
#include "filesystem/get_next_line.h"
#include "random.h"
#include "proc/proc.h"


static fs_fd	g_fds[FS_MAX_FDS] = {0};

static int	is_valid_open_flags(int flags)
{
	int access_mode;

	if (flags & ~(O_READ | O_WRITE | O_CREATE | O_APPEND | O_TRUNC))
		return (0);
	if ((flags & O_APPEND) && !(flags & O_WRITE))
		return (0);
	if ((flags & O_TRUNC) && !(flags & O_WRITE))
    	return 0;
	access_mode = flags & (O_READ | O_WRITE);
	if (access_mode == 0)
		return (0);
	return (1);
}

int		popen(const char *path, int flags, fs_fd *fds)
{
	int			fd;
	int			i;
	FAT32_File	file;

	rng_add_entropy(time_us()); // add entropy

	if (!path || !is_valid_open_flags(flags))
		return (-1);
	fd = -1;
	for (i = 0; i < FS_MAX_FDS; i++)
	{
		if (fds[i].mode == FILE_NOT_CREATED)
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

	if ((flags & O_TRUNC) && (flags & O_WRITE))
	{
		if (fat32_truncate(&file) != 0)
			return -1;
	}
	
	fds[fd].file = file;
	fds[fd].mode = (file_open_mode)(flags & (O_READ | O_WRITE | O_CREATE | O_APPEND | O_TRUNC));
	if (fds[fd].mode & O_APPEND)
		fds[fd].file.pos = fds[fd].file.size;
	
	return (fd);
}

int		open(const char *path, int flags)
{
	return popen(path, flags, g_fds);
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
	size_t	l1;
	size_t	l2;
	int add_slash;
	char *output;

	if (!path || !path2)
		return 0;

	l1 = ft_strlen(path);
	l2 = ft_strlen(path2);
	add_slash = (l1 > 0 && path[l1 - 1] != '/' && l2 > 0 && path2[0] != '/');

	output = kmalloc(l1 + l2 + add_slash + 1);
	if (!output)
		return 0;

	memcpy(output, path, l1);

	if (add_slash)
		output[l1] = '/';

	memcpy(output + l1 + add_slash, path2, l2);
	output[l1 + l2 + add_slash] = '\0';

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

int		dir_create(const char *path)
{
	FAT32_File	dir;

	if (!path)
		return (1);
	dir = fat32_mkdir(path);
	if (dir.first_cluster == 0 || !dir.is_dir)
		return (1);
	return (0);
}

int		dir_delete(const char *path)
{
	if (!path)
		return (1);
	if (!dir_exists(path) || fat32_rmdir(path) != 0)
		return (1);
	return (0);
}

int		file_rename(const char *old_path, const char *new_path)
{
	int		in_fd;
	int		out_fd;
	int		ret;
	uint32_t	rd;
	char		buf[512];

	if (!old_path || !new_path)
		return (-1);
	if (dir_exists(old_path) || file_exists(new_path) || dir_exists(new_path))
		return (-1);
	in_fd = open(old_path, O_READ);
	if (in_fd < 0)
		return (-1);
	out_fd = open(new_path, O_WRITE | O_CREATE | O_TRUNC);
	if (out_fd < 0)
	{
		close(in_fd);
		return (-1);
	}
	ret = 0;
	while (1)
	{
		rd = read(in_fd, buf, sizeof(buf));
		if (rd == (uint32_t)-1)
		{
			ret = -1;
			break;
		}
		if (rd == 0)
			break;
		if (write(out_fd, buf, rd) != (int)rd)
		{
			ret = -1;
			break;
		}
	}
	close(in_fd);
	close(out_fd);
	if (ret != 0)
	{
		file_delete(new_path);
		return (-1);
	}
	if (file_delete(old_path) != 0)
	{
		file_delete(new_path);
		return (-1);
	}
	return (0);
}

uint32_t	pread(int fd, char *buffer, uint32_t count, fs_fd *fds)
{
	if (fd >= FS_MAX_FDS || fd < 0 || fds[fd].mode == FILE_NOT_CREATED)
		return (-1);
	if (!(fds[fd].mode & O_READ))
		return (-1);
	
	return (fat32_read(&(fds[fd].file), (uint8_t *)buffer, count));
}

uint32_t	read(int fd, char *buffer, uint32_t count)
{
	return pread(fd, buffer, count, g_fds);
}


int		pwrite(int fd, const char *buffer, uint32_t count, fs_fd *fds)
{
	if (fd >= FS_MAX_FDS || fd < 0 || fds[fd].mode == FILE_NOT_CREATED)
		return (-1);

	if (!(fds)) // not in write mode
		return (-1);
	if (fds[fd].mode & O_APPEND)
		fds[fd].file.pos = fds[fd].file.size;
	
	return (fat32_write(&(fds[fd].file), (const uint8_t *)buffer, count));
}

int		write(int fd, const char *buffer, uint32_t count)
{
	return pwrite(fd, buffer, count, g_fds);
}

int		pclose(int fd, fs_fd *fds)
{
	if (fd >= FS_MAX_FDS || fd < 0 || fds[fd].mode == FILE_NOT_CREATED)
		return (-1);

	rng_add_entropy(time_us()); // add entropy

	fds[fd].mode = FILE_NOT_CREATED;
	fat32_close(&(fds[fd].file));
	clear_fd_buffer(fd); // used for GNL buffer clearing
	return 0;
}

int		close(int fd)
{
	return pclose(fd, g_fds);
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
