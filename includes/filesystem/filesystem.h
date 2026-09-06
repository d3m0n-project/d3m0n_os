#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"
#include "log.h"
#include "libft.h"
#include "filesystem/fat32.h"

#define FS_MAX_FDS		64
#define SECTOR_SIZE		512

typedef enum
{
	FILE_NOT_CREATED=0,
	O_READ=1,
	O_WRITE=2,
	O_CREATE=4,
	O_APPEND=8,
	O_TRUNC=16
}	file_open_mode;

typedef enum
{
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
}	e_seek_directive;

typedef struct fs_fd
{
	FAT32_File		file;
	file_open_mode	mode;
}	fs_fd;

int			block_read_multi(uint32_t lba, uint32_t count, uint8_t *buffer);
int			block_write_multi(uint32_t lba, uint32_t count, const uint8_t *buffer);

void		fat32_sync(void);
int			fat32_mount(int partition_id);
int			filesystem_ready(void);
int			sd_init(void);

char		*path_add(char *path, char *path2);
int			file_exists(const char *path);
int			dir_exists(const char *path);
int			file_delete(const char *path);
int			dir_create(const char *path);
int			dir_delete(const char *path);
int			file_rename(const char *old_path, const char *new_path);
int			open(const char *path, int flags);
uint32_t	read(int fd, char *buffer, uint32_t count);
int			write(int fd, const char *buffer, uint32_t count);
int			close(int fd);
void		list_dir(const char *path);
char		**list_dir_files(const char *path);
uint32_t	lseek(int fd, int32_t offset, e_seek_directive whence);

// process relative variants
int			popen(const char *path, int flags, fs_fd *fds);
uint32_t	pread(int fd, char *buffer, uint32_t count, fs_fd *fds);
int			pwrite(int fd, const char *buffer, uint32_t count, fs_fd *fds);
int			pclose(int fd, fs_fd *fds);

#endif
