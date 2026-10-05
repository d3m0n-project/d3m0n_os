#ifndef STDIO_HPP
#define STDIO_HPP

#include "types.h"
#include "sys.h"
#include "printf/printf.hpp"
#include "stdlib.h"
#include "memory.h"
#include "app/app_manifest.h"

#define stderr	2
#define stdout	1
//#define stdin	0 // TODO: stdin

#define BUFSIZ	1024

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		int fd;
	}	FILE;

	int		sprintf(char *str, const char *format, ...);
	int		snprintf(char *str, size_t size, const char *format, ...);
	size_t	printf(const char *format, ...);
	size_t	fprintf(int fd, const char *format, ...);
#ifdef __cplusplus
}
#endif

#endif