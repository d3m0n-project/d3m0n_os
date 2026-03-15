#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"
#include "log.h"

#define SECTOR_SIZE	512

typedef enum
{
	FILE_NOT_CREATED=0,
	O_READ=1,
	O_WRITE=2,
	O_CREATE=4
}	file_open_mode;

int		block_write(uint32_t lba, const uint8_t* buffer);
int		block_read(uint32_t lba, uint8_t* buffer);

int		fat32_mount(void);

int		sd_init(void);

int		open(const char *path, int flags);
int		read(int fd, char *buffer, uint32_t count);
int		write(int fd, const char *buffer, uint32_t count);
int		close(int fd);

#endif
