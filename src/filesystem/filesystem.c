#include "filesystem.h"

static file_entry_t	file_table[MAX_FILES];
static uint32_t		file_count;

void fs_init()
{
    disk_read(1, file_table);
}

int fs_find(const char *name)
{
    for(int i=0;i<file_count;i++)
        if(strcmp(file_table[i].name,name) == 0)
            return i;
    return -1;
}

int fs_read(const char *name, void *buffer)
{
    int id = fs_find(name);
    if(id < 0)
		return -1;

    disk_read(file_table[id].sector, buffer);
    return file_table[id].size;
}

int fs_write(const char *name, void *buffer, uint32_t size)
{
    int id = file_count++;

    strcpy(file_table[id].name, name);
    file_table[id].sector = 2 + id;
    file_table[id].size = size;
    file_table[id].timestamp = time_now();

    disk_write(file_table[id].sector, buffer);
    disk_write(1, file_table);

    return 0;
}