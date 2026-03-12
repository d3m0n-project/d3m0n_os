#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"

#define MAX_FILES 64

typedef struct
{
    char		name[32];
    uint32_t	sector;
    uint32_t	size;
    uint64_t	timestamp;
}	file_entry_t;

void	fs_init();
int		fs_read(const char *name, void *buffer);
int		fs_write(const char *name, void *buffer, uint32_t size);

#endif