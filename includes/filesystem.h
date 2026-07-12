#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"
#include "log.h"

#define FS_MAX_FDS		64
#define SECTOR_SIZE		512

typedef enum
{
	FILE_NOT_CREATED=0,
	O_READ=1,
	O_WRITE=2,
	O_CREATE=4,
	O_APPEND=8
}	file_open_mode;

typedef enum
{
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
}	e_seek_directive;

int			block_write(uint32_t lba, const uint8_t* buffer);
int			block_read(uint32_t lba, uint8_t* buffer);

int			fat32_mount(int partition_id);
int			filesystem_ready(void);
int			sd_init(void);

char		*path_add(char *path, char *path2);
int			open(const char *path, int flags);
int			file_exists(const char *path);
int			dir_exists(const char *path);
int			file_delete(const char *path);
uint32_t	read(int fd, char *buffer, uint32_t count);
int			write(int fd, const char *buffer, uint32_t count);
int			close(int fd);
void		list_dir(const char *path);
char		**list_dir_files(const char *path);
uint32_t	lseek(int fd, int32_t offset, e_seek_directive whence);

#endif
