#include "types.h"
#include "log.h"
#include "uart.h"
#include "usermode.h"
#include "proc.h"
#include "filesystem.h"
#include "d3m0n.h"
#include "display.h"

#define SYS_PRINT_MAX_LEN 512
#define SYSCALL_EXIT_INDEX 0

#define SYS_READ_INDEX 2
#define SYS_WRITE_INDEX 3
#define SYS_OPEN_INDEX 4
#define SYS_CLOSE_INDEX 5
#define SYS_UNAME_INDEX 6
#define SYS_GETDENTS_INDEX 7
#define SYS_RENAME_INDEX 8
#define SYS_MKDIR_INDEX 9
#define SYS_RMDIR_INDEX 10

extern t_process	*current_process;

static const char	*resolve_user_string_ptr(uint32_t user_ptr)
{
	uint32_t	base;
	uint32_t	size;

	if (!current_process || !current_process->address_space)
		return ((const char *)user_ptr);
	base = current_process->image_vaddr_base;
	size = current_process->image_size;
	if (size == 0)
		return ((const char *)user_ptr);
	if (user_ptr < base || user_ptr >= base + size)
		return ((const char *)user_ptr);
	return ((const char *)((char *)current_process->address_space + (user_ptr - base)));
}

static void	*resolve_user_ptr(uint32_t user_ptr, uint32_t len)
{
	uint32_t	base;
	uint32_t	size;
	uint64_t	end;

	if (!current_process || !current_process->address_space)
		return ((void *)user_ptr);
	base = current_process->image_vaddr_base;
	size = current_process->image_size;
	if (size == 0)
		return ((void *)user_ptr);
	if (user_ptr < base)
		return ((void *)user_ptr);
	end = (uint64_t)user_ptr + (uint64_t)len;
	if (end > (uint64_t)base + (uint64_t)size)
		return ((void *)user_ptr);
	return ((void *)((char *)current_process->address_space + (user_ptr - base)));
}

int	sys_exit(uint32_t mode, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	process_exit_current(mode);
	return (int)mode;
}

int	sys_print(uint32_t str, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	char			buffer[SYS_PRINT_MAX_LEN + 1];
	const char	*src = resolve_user_string_ptr(str);
	size_t			index = 0;

	if (!src)
		return -1;
	while (index < SYS_PRINT_MAX_LEN && src[index])
	{
		buffer[index] = src[index];
		index++;
	}
	buffer[index] = '\0';
	uart_print(buffer);
	return 0;
}

int	sys_read(uint32_t fd, uint32_t user_buf, uint32_t count, uint32_t a3)
{
	char	*buffer;

	(void)a3;
	buffer = (char *)resolve_user_ptr(user_buf, count);
	if (!buffer)
		return (-1);
	return (int)read((int)fd, buffer, count);
}

int	sys_write(uint32_t fd, uint32_t user_buf, uint32_t count, uint32_t a3)
{
	const char	*buffer;

	(void)a3;
	buffer = (const char *)resolve_user_ptr(user_buf, count);
	if (!buffer)
		return (-1);
	return write((int)fd, buffer, count);
}

int	sys_open(uint32_t user_path, uint32_t flags, uint32_t a2, uint32_t a3)
{
	const char	*path; // TODO: fd list for each process

	(void)a2;
	(void)a3;
	path = resolve_user_string_ptr(user_path);
	if (!path)
		return (-1);
	return open(path, (int)flags);
}

int	sys_close(uint32_t fd, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	return close((int)fd);
}

int	sys_uname(uint32_t user_buf, uint32_t buf_len, uint32_t a2, uint32_t a3)
{
	char		*dst;
	char		*name;
	uint32_t	len;

	(void)a2;
	(void)a3;
	name = ft_strjoin("d3m0n os ", KERNEL_VERSION);
	if (!name)
		name = "unknown";
	len = (uint32_t)ft_strlen(name);
	if (buf_len == 0 || len + 1 > buf_len)
		return (-1);
	dst = (char *)resolve_user_ptr(user_buf, buf_len);
	if (!dst)
		return (-1);
	ft_memcpy(dst, name, len + 1);
	return (0);
}

int	sys_getdents(uint32_t user_path, uint32_t user_out_buf, uint32_t out_len, uint32_t a3)
{
	char		**entries;
	char		*out;
	const char	*path;
	uint32_t	total;
	uint32_t	i;
	uint32_t	name_len;

	(void)a3;
	if (out_len == 0)
		return (-1);
	path = resolve_user_string_ptr(user_path);
	out = (char *)resolve_user_ptr(user_out_buf, out_len);
	if (!path || !out)
		return (-1);
	entries = list_dir_files(path);
	if (!entries)
		return (-1);
	total = 0;
	i = 0;
	while (entries[i])
	{
		name_len = (uint32_t)ft_strlen(entries[i]);
		if (total + name_len + 1 >= out_len)
			break;
		ft_memcpy(out + total, entries[i], name_len);
		total += name_len;
		out[total++] = '\n';
		i++;
	}
	if (total < out_len)
		out[total] = '\0';
	i = 0;
	while (entries[i])
		kfree(entries[i++]);
	kfree(entries);
	return (int)total;
}

int	sys_rename(uint32_t user_old_path, uint32_t user_new_path, uint32_t a2, uint32_t a3)
{
	const char	*old_path;
	const char	*new_path;

	(void)a2;
	(void)a3;
	old_path = resolve_user_string_ptr(user_old_path);
	new_path = resolve_user_string_ptr(user_new_path);
	if (!old_path || !new_path)
		return (-1);
	return file_rename(old_path, new_path);
}

int	sys_mkdir(uint32_t user_path, uint32_t a1, uint32_t a2, uint32_t a3)
{
	const char	*path;

	(void)a1;
	(void)a2;
	(void)a3;
	path = resolve_user_string_ptr(user_path);
	if (!path)
		return (-1);
	return dir_create(path);
}

int	sys_rmdir(uint32_t user_path, uint32_t a1, uint32_t a2, uint32_t a3)
{
	const char	*path;

	(void)a1;
	(void)a2;
	(void)a3;
	path = resolve_user_string_ptr(user_path);
	if (!path)
		return (-1);
	return dir_delete(path);
}

int	sys_sbrk(uint32_t increment, uint32_t a1, uint32_t a2, uint32_t a3)
{
	t_process *proc = current_process;
	uint32_t old_end;
	uint32_t new_end;
	int32_t signed_inc;

	(void)a1;
	(void)a2;
	(void)a3;

	if (!proc)
		return (int)-1;

	old_end = proc->heap_end;

	if (proc->heap_start == 0)
		return (int)-1;

	signed_inc = (int32_t)increment;

	/* sbrk(0) returns the heap base. This gives user-space the stable,
	 * static-free anchor it needs to locate its allocator control block. */
	if (signed_inc == 0)
		return (int)proc->heap_start;

	new_end = (uint32_t)(old_end + signed_inc);

	if (new_end < proc->heap_start)
		return (int)-1;

	if (new_end > USER_HEAP_MAX(proc))
		return (int)-1;

	proc->heap_end = new_end;

	return (int)old_end;
}

int	sys_getfbaddr(uint32_t buff_ptr, uint32_t width_ptr, uint32_t height_ptr, uint32_t a3)
{
	(void)a3;
	uint8_t	**fb = (uint8_t **)resolve_user_ptr(buff_ptr, 1);
	int	*width = (int *)resolve_user_ptr(width_ptr, 1);
	int	*height =  (int *)resolve_user_ptr(height_ptr, 1);
	if (!fb || !width || !height)
		return -1;

	*width = SCREEN_WIDTH;
	*height = SCREEN_HEIGHT;
	//*fb = // TODO: fb

	return 0;
}
int	sys_flushfb(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a0;
	(void)a1;
	(void)a2;
	(void)a3;

	// TODO: flush

	return 0;
}



syscall_t	syscall_table[] = {
	sys_exit,
	sys_print,
	sys_read,
	sys_write,
	sys_open,
	sys_close,
	sys_uname,
	sys_getdents,
	sys_rename,
	sys_mkdir,
	sys_rmdir,
	sys_sbrk,
	sys_getfbaddr,
	sys_flushfb
};

int	syscall_dispatch(uint32_t number, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	if (number >= sizeof(syscall_table)/sizeof(syscall_table[0]))
		return -1;
	return syscall_table[number](a0, a1, a2, a3);
}

int	syscall_handler(syscall_frame_t *frame)
{
	int ret = syscall_dispatch(frame->r7, frame->r0, frame->r1, frame->r2, frame->r3);
	if (frame->r7 == SYSCALL_EXIT_INDEX && !current_process)
		return 1;

	frame->r0 = ret;
	return 0;
}