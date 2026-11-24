#ifndef SDK_LOG_H
#define SDK_LOG_H

#include "types.h"

enum
{
	LOG_NONE = 0,
	LOG_SUCCESS = 1,
	LOG_ERROR = 2,
	LOG_WARNING = 4,
	LOG_INFO = 8,
	LOG_INDENT = 16
};

/* SDK log calls use the kernel's (format, level, ...) convention. Applications
 * expose printf instead, so discard the level while preserving format args. */
#ifndef KERNEL_VERSION
# define log(format, type, ...) printf((format), ##__VA_ARGS__)
#endif

#endif
