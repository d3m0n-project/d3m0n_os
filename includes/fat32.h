#ifndef FAT32_H
#define FAT32_H

#include "types.h"

typedef struct
{
	uint32_t	first_cluster;
	uint32_t	size;
	uint32_t	pos;
	uint32_t	dir_cluster;
	uint32_t	dir_index;
	uint8_t		is_dir;
}	FAT32_File;

FAT32_File	fat32_open(const char *filename);
FAT32_File	fat32_create(const char *filename);
int			fat32_read(FAT32_File *file, uint8_t *buffer, uint32_t len);
int			fat32_write(FAT32_File *file, const uint8_t *buffer, uint32_t len);
void		fat32_close(FAT32_File *file);
void		fat32_list_dir(const char *path, void (*print)(const char *name, uint32_t size));

#endif