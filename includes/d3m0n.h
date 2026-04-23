#ifndef D3M0N_H
#define D3M0N_H

// kernel related data
#ifndef KERNEL_VERSION
	#define KERNEL_VERSION (const char *)"2.0.0"
#endif
#ifndef KERNEL_VERSION_NAME
	#define KERNEL_VERSION_NAME (const char *)"outset"
#endif

#ifndef DEBUG
	#define DEBUG 0
#endif

void	panic(const char *message);

#endif