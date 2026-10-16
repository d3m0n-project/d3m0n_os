#ifndef SYS_H
#define SYS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
	static inline	uint32_t	syscall(uint32_t number, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
	{
		register uint32_t _r0 __asm__("r0") = (uint32_t)(a0);
		register uint32_t _r1 __asm__("r1") = (uint32_t)(a1);
		register uint32_t _r2 __asm__("r2") = (uint32_t)(a2);
		register uint32_t _r3 __asm__("r3") = (uint32_t)(a3);
		register uint32_t _r7 __asm__("r7") = (uint32_t)(number);
		__asm__ volatile(
			"svc #0"
			: "+r"(_r0)
			: "r"(_r1), "r"(_r2), "r"(_r3), "r"(_r7)
			: "memory", "cc"
		);
		return (uint32_t)_r0;                                             
	}

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


	typedef enum: uint32_t
	{
		SYS_EXIT,
		SYS_READ,
		SYS_WRITE,
		SYS_OPEN,
		SYS_CLOSE,
		SYS_LSEEK,
		SYS_UNAME,
		SYS_GETDENTS,
		SYS_RENAME,
		SYS_MKDIR,
		SYS_RMDIR,
		SYS_REMOVE,
		SYS_SBRK,
		SYS_SURFACE_CREATE,
		SYS_SURFACE_UPDATE,
		SYS_GET_TIME,
		SYS_RANDOM,
		SYS_EXEC,
		SYS_KILL,
		SYS_PSTATE,


		TMP_TOTAL_SYSCALL_COUNT // do not remove me
	}   e_syscall_indexes;

	#define TOTAL_SYSCALL_COUNT	(TMP_TOTAL_SYSCALL_COUNT + 1)

	#define PROC_READY			0
	#define PROC_RUNNING		1
	#define PROC_BLOCKED		2
	#define PROC_SLEEPING		3
	#define PROC_ZOMBIE			4
	#define PROC_CRASHED		5

	static inline void __attribute__((noreturn)) exit(int ret_code)
	{
		syscall(SYS_EXIT, (uint32_t)ret_code, (uint32_t)0, (uint32_t)0, (uint32_t)0);
		while(1);
	}

	static inline int	read(int fd, char *buffer, uint32_t count)
	{
		return (int)syscall(SYS_READ, (uint32_t)fd, (uint32_t)buffer, count, (uint32_t)0);
	}

	static inline int	write(int fd, const char *buffer, uint32_t count)
	{
		return (int)syscall(SYS_WRITE, (uint32_t)fd, (uint32_t)buffer, count, (uint32_t)0);
	}

	static inline int	open(const char *path, uint32_t flags)
	{
		return (int)syscall(SYS_OPEN, (uint32_t)path, (uint32_t)flags, (uint32_t)0, (uint32_t)0);
	}

	static inline int	close(int fd)
	{
		return (int)syscall(SYS_CLOSE, (uint32_t)fd, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline int	lseek(int fd, size_t offset, e_seek_directive whence)
	{
		return (int)syscall(SYS_LSEEK, (uint32_t)fd, (uint32_t)offset, (uint32_t)whence, (uint32_t)0);
	}

	static inline int	uname(char *buffer, uint32_t buffer_len)
	{
		return (int)syscall(SYS_UNAME, (uint32_t)buffer, buffer_len, (uint32_t)0, (uint32_t)0);
	}

	static inline int	getdents(const char *path, char *buffer, uint32_t buffer_len)
	{
		return (int)syscall(SYS_GETDENTS, (uint32_t)path, (uint32_t)buffer, buffer_len, (uint32_t)0);
	}

	static inline int	rename(const char *old_path, const char *new_path)
	{
		return (int)syscall(SYS_RENAME, (uint32_t)old_path, (uint32_t)new_path, (uint32_t)0, (uint32_t)0);
	}

	static inline int	mkdir(const char *path)
	{
		return (int)syscall(SYS_MKDIR, (uint32_t)path, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline int	rmdir(const char *path)
	{
		return (int)syscall(SYS_RMDIR, (uint32_t)path, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline int	remove(const char *path)
	{
		return (int)syscall(SYS_REMOVE, (uint32_t)path, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline int	surface_create(uint32_t width, uint32_t height, uint8_t **surface, int *pitch)
	{
		return (int)syscall(SYS_SURFACE_CREATE, width, height, (uint32_t)surface, (uint32_t)pitch);
	}

	static inline int	surface_update(uint8_t *surface)
	{
		return (int)syscall(SYS_SURFACE_UPDATE, (uint32_t)surface, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline void	*sbrk(size_t increment)
	{
		return (void *)syscall(SYS_SBRK, increment, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline uint32_t	time(void *timezone)
	{
		return syscall(SYS_GET_TIME, (uint32_t)timezone, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline uint32_t	clock(void)
	{
		return syscall(SYS_GET_TIME, (uint32_t)0, (uint32_t)1, (uint32_t)0, (uint32_t)0);
	}

	static inline uint32_t	random_u32(void)
	{
		return syscall(SYS_RANDOM, (uint32_t)0, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

	static inline uint32_t	exec(const char *path, const char **argv)
	{
		return syscall(SYS_EXEC, (uint32_t)path, (uint32_t)argv, (uint32_t)0, (uint32_t)0);
	}

	static inline uint32_t	kill(uint32_t pid, int code)
	{
		return syscall(SYS_KILL, (uint32_t)pid, (uint32_t)code, (uint32_t)0, (uint32_t)0);
	}

	static inline uint32_t	pstate(uint32_t pid)
	{
		return syscall(SYS_PSTATE, (uint32_t)pid, (uint32_t)0, (uint32_t)0, (uint32_t)0);
	}

#ifdef __cplusplus
}
#endif

#endif
