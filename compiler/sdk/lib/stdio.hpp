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


#ifdef __cplusplus
extern "C" {
#endif
	int	sprintf(char *str, const char *format, ...);
	size_t	printf(const char *format, ...);
	size_t	fprintf(int fd, const char *format, ...);
#ifdef __cplusplus
}
#endif

#endif