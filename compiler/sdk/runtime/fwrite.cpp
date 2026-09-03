#include "sys.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		int fd;
	}	FILE;

	size_t	fwrite(void *buffer, size_t blockSize, size_t blockCount, FILE *stream)
	{
		if (blockSize == 0)
			return 0;
		return ((size_t)write(stream->fd, (const char *)buffer, blockSize*blockCount) / blockSize);
	}
#ifdef __cplusplus
}
#endif