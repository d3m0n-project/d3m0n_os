#ifndef STDIO_H
#define STDIO_H

#include "types.h"
#include "sys.h"
#include "printf/printf.hpp"
#include "stdlib.h"
#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif
	char	*sprintf(const char *format, ...);
	size_t	printf(const char *format, ...);
#ifdef __cplusplus
}
#endif

#endif