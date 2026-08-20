#ifndef SYS_H
#define SYS_H

#include "types.h"

typedef enum
{
	SYS_EXIT,
	SYS_READ,
	SYS_WRITE,
	SYS_OPEN,
	SYS_CLOSE,
	SYS_UNAME,
	SYS_GETDENTS,
	SYS_RENAME,
	SYS_MKDIR,
	SYS_RMDIR,
	SYS_SBRK,
	SYS_GETFBADDR,
	SYS_FLUSHFB
}   e_syscall_indexes;

static inline int	syscall(uint32_t number, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	register uint32_t r0 asm("r0") = a0;
	register uint32_t r1 asm("r1") = a1;
	register uint32_t r2 asm("r2") = a2;
	register uint32_t r3 asm("r3") = a3;
	register uint32_t r7 asm("r7") = number;

	asm volatile(
		"svc #0"
		: "+r"(r0)
		: "r"(r1),
		  "r"(r2),
		  "r"(r3),
		  "r"(r7)
		: "memory"
	);
	return (int)r0;
}

static inline void	exit(int ret_code)
{
	syscall(SYS_EXIT, (uint32_t)ret_code, 0, 0, 0);
}

static inline int	read(int fd, char *buffer, uint32_t count)
{
	return syscall(SYS_READ, (uint32_t)fd, (uint32_t)buffer, count, 0);
}

static inline int	write(int fd, const char *buffer, uint32_t count)
{
	return syscall(SYS_WRITE, (uint32_t)fd, (uint32_t)buffer, count, 0);
}

static inline int	open(const char *path, int flags)
{
	return syscall(SYS_OPEN, (uint32_t)path, (uint32_t)flags, 0, 0);
}

static inline int	close(int fd)
{
	return syscall(SYS_CLOSE, (uint32_t)fd, 0, 0, 0);
}

static inline int	uname(char *buffer, uint32_t buffer_len)
{
	return syscall(SYS_UNAME, (uint32_t)buffer, buffer_len, 0, 0);
}

static inline int	getdents(const char *path, char *buffer, uint32_t buffer_len)
{
	return syscall(SYS_GETDENTS, (uint32_t)path, (uint32_t)buffer, buffer_len, 0);
}

static inline int	rename(const char *old_path, const char *new_path)
{
	return syscall(SYS_RENAME, (uint32_t)old_path, (uint32_t)new_path, 0, 0);
}

static inline int	mkdir(const char *path)
{
	return syscall(SYS_MKDIR, (uint32_t)path, 0, 0, 0);
}

static inline int	rmdir(const char *path)
{
	return syscall(SYS_RMDIR, (uint32_t)path, 0, 0, 0);
}

static inline int	getfbaddr(uint8_t	**fb_addr, int *width, int *height, int *pitch)
{
	return syscall(SYS_GETFBADDR, (uint32_t)fb_addr, (uint32_t)width, (uint32_t)height, (uint32_t)pitch);
}

static inline int	flushfb(void)
{
	return syscall(SYS_FLUSHFB, 0, 0, 0, 0);
}

static inline	int	sbrk(size_t increment)
{
	return syscall(SYS_SBRK, increment, 0, 0, 0);
}

#endif
