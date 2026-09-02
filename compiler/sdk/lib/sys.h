#ifndef SYS_H
#define SYS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
	#define syscall(number, a0, a1, a2, a3) ({                \
		register uint32_t _r0 asm("r0") = (uint32_t)(a0);     \
		register uint32_t _r1 asm("r1") = (uint32_t)(a1);     \
		register uint32_t _r2 asm("r2") = (uint32_t)(a2);     \
		register uint32_t _r3 asm("r3") = (uint32_t)(a3);     \
		register uint32_t _r7 asm("r7") = (uint32_t)(number); \
		asm volatile(                                         \
			"svc #0"                                          \
			: "+r"(_r0)                                       \
			: "r"(_r1), "r"(_r2), "r"(_r3), "r"(_r7)          \
			: "memory", "cc"                                  \
		);                                                    \
		(int)_r0;                                             \
	})

	typedef enum
	{
		O_READ=1,
		O_WRITE=2,
		O_CREATE=4,
		O_APPEND=8,
		O_TRUNC=16
	}	file_open_mode;

	typedef enum
	{
		SEEK_SET,
		SEEK_CUR,
		SEEK_END
	}	e_seek_directive;



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
		SYS_SURFACE_CREATE,
		SYS_SURFACE_UPDATE
	}   e_syscall_indexes;

	static inline void __attribute__((noreturn)) exit(int ret_code)
	{
		syscall(SYS_EXIT, (uint32_t)ret_code, 0, 0, 0);
		while(1);
	}

	static inline int	read(int fd, char *buffer, uint32_t count)
	{
		return syscall(SYS_READ, (uint32_t)fd, (uint32_t)buffer, count, 0);
	}

	static inline int	write(int fd, const char *buffer, uint32_t count)
	{

		return syscall(SYS_WRITE, (uint32_t)fd, (uint32_t)buffer, count, 0);
	}

	static inline int	open(const char *path, file_open_mode flags)
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

	static inline int	surface_create(uint32_t width, uint32_t height, uint8_t **surface, int *pitch)
	{
		return syscall(SYS_SURFACE_CREATE, width, height, (uint32_t)surface, (uint32_t)pitch);
	}

	static inline int	surface_update(uint8_t *surface)
	{
		return syscall(SYS_SURFACE_UPDATE, (uint32_t)surface, 0, 0, 0);
	}

	static inline	int	sbrk(size_t increment)
	{
		return syscall(SYS_SBRK, increment, 0, 0, 0);
	}

#ifdef __cplusplus
}
#endif

#endif
