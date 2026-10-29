#ifndef IMAGE_READ_H
#define IMAGE_READ_H

#include "sys.h"
#include "memory.h"

#define MAX_IMAGE_SIZE		262144

static inline unsigned char	*image_read_to_buf(const char *path, size_t *buf_size)
{
	*buf_size = 0;
	if (!path)
		return 0;
	
	int fd = open(path, O_READ);
	if (fd < 0)
		return 0;
	
	int file_size = lseek(fd, 0, SEEK_END);
	if (file_size <= 0 || file_size > 8388608 || lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return 0;
	}
	unsigned char *data = (unsigned char *)malloc((size_t)file_size);
	if (!data)
	{
		close(fd);
		return 0;
	}
	int offset = 0;
	while (offset < file_size)
	{
		int amount = read(fd, (char *)data + offset, file_size - offset);
		if (amount <= 0)
		{
			free(data);
			close(fd);
			return 0;
		}
		offset += amount;
	}
	close(fd);
	*buf_size = file_size;
	return data;
}

#endif