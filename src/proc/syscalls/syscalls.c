#include "types.h"
#include "log.h"
#include "protocol/uart.h"
#include "proc/usermode.h"
#include "proc/proc.h"
#include "filesystem/filesystem.h"
#include "d3m0n.h"
#include "display/display.h"
#include "peripheral/usb.h"
#include "proc/proc.h"
#include "time.h"
#include "random.h"

#define SYS_PRINT_MAX_LEN	512
#define SYSCALL_EXIT_INDEX	0
#define SYSCALL_THREAD_KILL_INDEX	24
#define SYSCALL_THREAD_EXIT_INDEX	25
#define STDIN_QUEUE_SIZE	256
#define SYS_READ_RETRY		-2

extern t_process	*current_process;
extern void			draw_topbar(void);

static char		g_stdin_queue[STDIN_QUEUE_SIZE];
static uint32_t	g_stdin_head = 0;
static uint32_t	g_stdin_tail = 0;
static uint32_t	g_stdin_count = 0;

static void	stdin_queue_push(int c)
{
	if (c < 0)
		return;
	if (g_stdin_count == STDIN_QUEUE_SIZE)
	{
		g_stdin_head = (g_stdin_head + 1) % STDIN_QUEUE_SIZE;
		g_stdin_count--;
	}
	g_stdin_queue[g_stdin_tail] = (char)c;
	g_stdin_tail = (g_stdin_tail + 1) % STDIN_QUEUE_SIZE;
	g_stdin_count++;
}

static int	stdin_queue_pop(int *c)
{
	if (!c || g_stdin_count == 0)
		return 0;
	*c = (unsigned char)g_stdin_queue[g_stdin_head];
	g_stdin_head = (g_stdin_head + 1) % STDIN_QUEUE_SIZE;
	g_stdin_count--;
	return 1;
}

static void	stdin_usb_key_handler(int key, bool pressed, uint32_t modifiers)
{
	(void)modifiers;
	if (!pressed || key > 127)
		return;
	stdin_queue_push(key);
}

static void	stdin_poll_sources(void)
{
	int c;

	c = uart_getc_nb();
	if (c >= 0)
	{
		if (c == '\r')
			c = '\n';
		stdin_queue_push(c);
	}
	usb_keyboard_poll(stdin_usb_key_handler);
}

static t_process *thread_memory_owner(t_process *proc)
{
	if (!proc || !proc->is_thread)
		return proc;
	for (t_process *candidate = scheduled_processes; candidate; candidate = candidate->next)
		if (candidate->pid == proc->thread_owner_pid && !candidate->is_thread)
			return candidate;
	return proc;
}

static void	*resolve_user_ptr(uint32_t user_ptr, uint32_t len)
{
	t_process *proc = current_process;
	t_process *memory_owner = thread_memory_owner(proc);
	if (!proc)
		return (void *)user_ptr;

	// ELF image
	if (proc->image_size > 0 && user_ptr >= proc->image_vaddr_base && user_ptr < proc->image_vaddr_base + proc->image_size)
	{
		if ((uint64_t)user_ptr + len > (uint64_t)proc->image_vaddr_base + proc->image_size)
			return 0;
		return (uint8_t *)proc->image_backing + (user_ptr - proc->image_vaddr_base);
	}

	// user stack
	if (proc->user_stack)
	{
		uint32_t lo = (uint32_t)proc->user_stack;
		uint32_t hi = lo + USER_STACK_PAGES * PAGE_SIZE;
		if (user_ptr >= lo && user_ptr < hi)
		{
			if ((uint64_t)user_ptr + len > hi)
				return 0;
			return (void *)user_ptr;
		}
	}

	// user heap
	if (memory_owner->heap_start && user_ptr >= memory_owner->heap_start && user_ptr < memory_owner->heap_end)
	{
		if ((uint64_t)user_ptr + len > memory_owner->heap_end)
			return 0;
		return (uint8_t *)memory_owner->image_backing + (user_ptr - memory_owner->image_vaddr_base);
	}

	log("resolve_user_ptr: ptr %p not in any valid region\n", LOG_WARNING, user_ptr);
	return 0;
}

static const char *resolve_user_string_ptr(uint32_t user_ptr)
{
	return (const char *)resolve_user_ptr(user_ptr, 1);
}

int	sys_exit(uint32_t mode, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	process_exit_current(mode);
	return (int)mode;
}

int	sys_read(uint32_t fd, uint32_t user_buf, uint32_t count, uint32_t a3)
{
	char	*buffer;
	uint32_t i;
	int		c;

	(void)a3;
	if (fd == 0)
	{
		if (count == 0)
			return 0;
		buffer = (char *)resolve_user_ptr(user_buf, count);
		if (!buffer)
			return -1;
		if (current_process->stdio_redirected[0])
		{
			if (!(current_process->stdio_fds[0].mode & O_READ))
				return -1;
			return (int)fat32_read(&current_process->stdio_fds[0].file, (uint8_t *)buffer, count);
		}
		if (current_process->parent_pid)
		{
			for (i = 0; i < count; ++i)
			{
				uint32_t irq_state = disable_interrupts();
				if (!current_process->terminal_input_count)
				{
					restore_interrupts(irq_state);
					return i ? (int)i : SYS_READ_RETRY;
				}
				uint8_t c = current_process->terminal_input[current_process->terminal_input_head];
				current_process->terminal_input_head = (current_process->terminal_input_head + 1) % sizeof(current_process->terminal_input);
				current_process->terminal_input_count--;
				restore_interrupts(irq_state);
				buffer[i] = (char)c;
				if (c == '\n')
					return (int)(i + 1);
			}
			return (int)i;
		}
		stdin_poll_sources();
		for (i = 0; i < count; ++i)
		{
			if (!stdin_queue_pop(&c))
				return i ? (int)i : SYS_READ_RETRY;
			if (c == '\r')
				c = '\n';
			buffer[i] = (char)c;
			if (c == '\n')
				return (int)(i + 1);
		}
		return (int)i;
	}
	buffer = (char *)resolve_user_ptr(user_buf, count);
	if (!buffer)
		return (-1);

	if (fd < 3)
		return -1;

	return (int)pread((int)fd - 3, buffer, count, current_process->fds);
}

int sys_write(uint32_t fd, uint32_t user_buf, uint32_t count, uint32_t a3)
{
	(void)a3;
	//log("\nwriting '%s' l=%lu to %lu\n", 0, resolve_user_ptr(user_buf, count), count, fd);

	if (fd == 0)
		return -1;

	if (fd == 1 || fd == 2)
	{
		const char *buffer = (const char *)resolve_user_ptr(user_buf, count);
		if (!buffer)
			return -1;
		if (current_process->stdio_redirected[fd])
		{
			if (!(current_process->stdio_fds[fd].mode & O_WRITE))
				return -1;
			return (int)fat32_write(&current_process->stdio_fds[fd].file, (const uint8_t *)buffer, count);
		}
		if (current_process->parent_pid)
		{
			t_process *parent = scheduled_processes;
			while (parent && parent->pid != current_process->parent_pid)
				parent = parent->next;
			if (parent)
			{
				uint32_t written = 0;
				while (written < count)
				{
					uint32_t chunk = count - written;
					if (chunk > sizeof(uint64_t) + 3 * sizeof(int32_t))
						chunk = sizeof(uint64_t) + 3 * sizeof(int32_t);
					event_t event = {.type=EVENT_TERMINAL_OUTPUT, .timestamp=0, .x=0, .y=0,
						.key=0, .modifiers=chunk};
					memcpy(&event.timestamp, buffer + written, chunk);
					uint32_t irq_state = disable_interrupts();
					bool queued = parent->events.count < PROC_MAX_EVENTS - 1 && fifo_push(&parent->events, &event);
					restore_interrupts(irq_state);
					if (queued)
						written += chunk;
					else
					{
						uart_print_n(buffer + written, chunk);
						written += chunk;
					}
				}
				return (int)written;
			}
		}

		uint32_t written = 0;
		while (written < count)
		{
			uint32_t chunk = count - written;
			if (chunk > SYS_PRINT_MAX_LEN)
				chunk = SYS_PRINT_MAX_LEN;

			uart_print_n(buffer + written, chunk);
			written += chunk;
		}
		return (int)written;
	}

	const char *buffer = (const char *)resolve_user_ptr(user_buf, count);
	if (!buffer)
		return -1;

	return pwrite((int)fd - 3, buffer, count, current_process->fds);
}

int	sys_open(uint32_t user_path, uint32_t flags, uint32_t a2, uint32_t a3)
{
	const char	*path; // TODO: fd list for each process

	(void)a2;
	(void)a3;
	path = resolve_user_string_ptr(user_path);
	if (!path)
		return (-1);

	int fd = popen(path, (int)flags, current_process->fds);
	if (fd == -1)
		return -1;
	return (fd + 3);
}

int	sys_close(uint32_t fd, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	if (fd < 3)
		return -1;

	return pclose((int)fd - 3, current_process->fds);
}

int	sys_lseek(uint32_t fd, uint32_t offset, uint32_t whence, uint32_t a3)
{
	(void)a3;
	if (fd < 3)
		return -1;
	return plseek((int)fd - 3, offset, (e_seek_directive)whence, current_process->fds);
}

int	sys_uname(uint32_t user_buf, uint32_t buf_len, uint32_t a2, uint32_t a3)
{
	char		*dst;
	char		*name;
	uint32_t	len;

	(void)a2;
	(void)a3;
	name = strjoin("d3m0n os ", KERNEL_VERSION);
	if (!name)
		name = "unknown";
	len = (uint32_t)strlen(name);
	if (buf_len == 0 || len + 1 > buf_len)
		return (-1);
	dst = (char *)resolve_user_ptr(user_buf, buf_len);
	if (!dst)
		return (-1);
	memcpy(dst, name, len + 1);
	return (0);
}

int	sys_getdents(uint32_t user_path, uint32_t user_out_buf, uint32_t out_len, uint32_t a3)
{
	char		**entries;
	uint32_t	*out_entries;
	char		*out_strings;
	const char	*path;
	uint64_t	string_bytes;
	uint64_t	table_bytes;
	uint32_t	i;
	uint32_t	name_len;

	(void)a3;
	if (out_len < sizeof(uint32_t))
		return (-1);
	path = resolve_user_string_ptr(user_path);
	out_entries = (uint32_t *)resolve_user_ptr(user_out_buf, out_len);
	if (!path || !out_entries)
		return (-1);
	entries = list_dir_files(path);
	if (!entries)
		return (-1);
	string_bytes = 0;
	i = 0;
	while (entries[i])
	{
		name_len = (uint32_t)strlen(entries[i]);
		table_bytes = (uint64_t)(i + 2) * sizeof(uint32_t);
		if (table_bytes + string_bytes + name_len + 1 > out_len)
			break;
		string_bytes += name_len + 1;
		i++;
	}
	table_bytes = (uint64_t)(i + 1) * sizeof(uint32_t);
	out_strings = (char *)out_entries + table_bytes;
	string_bytes = 0;
	for (uint32_t entry = 0; entry < i; entry++)
	{
		name_len = (uint32_t)strlen(entries[entry]);
		out_entries[entry] = user_out_buf + (uint32_t)table_bytes + (uint32_t)string_bytes;
		memcpy(out_strings + string_bytes, entries[entry], name_len + 1);
		string_bytes += name_len + 1;
	}
	out_entries[i] = 0;
	i = 0;
	while (entries[i])
		kfree(entries[i++]);
	kfree(entries);
	return (int)(table_bytes / sizeof(uint32_t) - 1);
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

int	sys_remove(uint32_t user_path, uint32_t a1, uint32_t a2, uint32_t a3)
{
	const char	*path;

	(void)a1;
	(void)a2;
	(void)a3;
	path = resolve_user_string_ptr(user_path);
	if (!path)
		return (-1);
	return file_delete(path);
}

int	sys_sbrk(uint32_t increment, uint32_t a1, uint32_t a2, uint32_t a3)
{
	t_process *proc = thread_memory_owner(current_process);
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
	if (signed_inc == 0)
		return (int)proc->heap_end;

	new_end = (uint32_t)(old_end + signed_inc);

	if (new_end < proc->heap_start)
		return (int)-1;

	if (new_end > USER_HEAP_MAX(proc))
		return (int)-1;

	proc->heap_end = new_end;
	return (int)old_end;
}

int	sys_surface_create(uint32_t width, uint32_t height, uint32_t surface_ptr, uint32_t pitch_ptr)
{
	t_process *proc = current_process;
	uint8_t **surface = (uint8_t **)resolve_user_ptr(surface_ptr, sizeof(*surface));
	int *pitch = (int *)resolve_user_ptr(pitch_ptr, sizeof(*pitch));
	uint32_t size;
	uint8_t *address;
	uint8_t *snapshot;

	// TODO: allow non fullscreen surfaces
	if (!proc || !surface || !pitch || width != SCREEN_WIDTH || height != SCREEN_HEIGHT - TOPBAR_HEIGHT || proc->surface_addr != 0)
		return -1;

	if (height > (uint32_t)-1 / width / sizeof(uint32_t))
		return -1;

	size = width * height * sizeof(uint32_t);
	address = (uint8_t *)kmalloc(size);
	if (!address)
		return -1;
	snapshot = (uint8_t *)kmalloc(size);
	if (!snapshot)
	{
		kfree(address);
		return -1;
	}
	if (mmu_map_user_range(&proc->address_space, address, size, 1, 0))
	{
		kfree(snapshot);
		kfree(address);
		return -1;
	}
	memset(address, 0, size);
	proc->surface_addr = (uint32_t)address;
	proc->surface_size = size;
	proc->surface_snapshot = (uint32_t)snapshot;
	proc->surface_width = width;
	proc->surface_height = height;
	*surface = address;
	*pitch = (int)(width * sizeof(uint32_t));
	return 0;
}

t_process	*last_updated_proc = 0;
int	sys_surface_update(uint32_t surface_addr, uint32_t a1, uint32_t a2, uint32_t a3)
{
	t_process	*proc = current_process;
	uint8_t		*surface;
	uint8_t		*framebuffer;
	uint8_t		*snapshot;
	int			pitch;
	uint32_t	row;
	uint32_t	start;
	int			full_refresh;

	(void)a1;
	(void)a2;
	(void)a3;
	if (!proc || surface_addr != proc->surface_addr || !proc->surface_size)
		return -1;
	surface = (uint8_t *)(uintptr_t)surface_addr;
	snapshot = (uint8_t *)(uintptr_t)proc->surface_snapshot;
	framebuffer = (uint8_t *)get_fb_addr(&pitch);
	if (!framebuffer || !snapshot || pitch < (int)(proc->surface_width * sizeof(uint32_t)))
		return -1;

	draw_topbar();
	/* The topbar writes straight into the framebuffer; write those dirty
	   cache lines back so the GPU scanout sees them too. */
	mmu_clean_invalidate_dcache_range((uint32_t)(uintptr_t)framebuffer, (uint32_t)TOPBAR_HEIGHT * pitch);
	full_refresh = (last_updated_proc != proc);
	for (row = 0; row < proc->surface_height; row++)
	{
		uint32_t *src = (uint32_t *)surface + row * proc->surface_width;
		uint32_t *old = (uint32_t *)snapshot + row * proc->surface_width;
		uint32_t x = 0;
		while (x < proc->surface_width)
		{
			while (x < proc->surface_width && !full_refresh && src[x] == old[x])
				x++;
			if (x == proc->surface_width)
				break;
			start = x;
			while (x < proc->surface_width && (full_refresh || src[x] != old[x]))
			{
				old[x] = src[x];
				x++;
			}
			memcpy(framebuffer + (row + TOPBAR_HEIGHT) * pitch + start * sizeof(uint32_t), src + start, (x - start) * sizeof(uint32_t));
			/* Clean the D-cache line so the GPU/display scanout (DMA) sees the pixels */
			mmu_clean_invalidate_dcache_range((uint32_t)(uintptr_t)(framebuffer + (row + TOPBAR_HEIGHT) * pitch + start * sizeof(uint32_t)), (x - start) * sizeof(uint32_t));
		}
	}
	#if DEBUG_OUTLINE == 1
	draw_rect_outline(0, TOPBAR_HEIGHT, proc->surface_width, proc->surface_height, OUTLINE_COLOR);
	#endif
	/* The app's surface copy may have overwritten the kernel-drawn mouse
	   cursor; repaint it on top so it is always visible. */
	usb_mouse_redraw();
	last_updated_proc = proc;
	return 0;
}

int	sys_get_time(uint32_t timezone_ptr, uint32_t output_ptr, uint32_t relative, uint32_t a3)
{
	(void)a3;
	if (!output_ptr)
		return -1;
	
	uint64_t	*output = (uint64_t *)resolve_user_ptr(output_ptr, sizeof(uint64_t *));
	
	 // TODO: timezone
	if (relative == 1)
		return (uint32_t)(time_from_boot() / CLOCKS_PER_SEC);

	if (!output)
		return -1;

	(void)timezone_ptr;
	*output = time_us();

	return 0;
}

int	sys_random(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a0;
	(void)a1;
	(void)a2;
	(void)a3;
	return random_u32();
}

int	sys_exec(uint32_t path_ptr, uint32_t argv_ptr, uint32_t a2, uint32_t a3)
{
	(void)a3;

	if (!path_ptr)
		return 0;

	const char *elf_path = (const char *)resolve_user_ptr(path_ptr, sizeof(const char));
	const char **raw_argv = 0;
	if (argv_ptr)
		raw_argv = (const char **)resolve_user_ptr(argv_ptr, sizeof(const char **));
	if (!elf_path)
		return 0;

	int argv_len = 0;
	while (raw_argv && raw_argv[argv_len])
		argv_len++;

	// allocate a copy of raw_argv but with resolved user ptr
	const char **resolved_argv = calloc(argv_len + 1, sizeof(char *));
	if (!resolved_argv)
	{
		log("SYS_EXEC: Could not allocate memory for resolved argv pointers\n", LOG_ERROR);
		return 0;
	}
	for (int i=0; i<argv_len; i++)
		resolved_argv[i] = (const char *)resolve_user_ptr((uint32_t)(raw_argv[i]), sizeof(const char *));
	int *source_fds = 0;
	if (a2)
	{
		source_fds = (int *)resolve_user_ptr(a2, sizeof(int) * 3);
		if (!source_fds)
		{
			kfree(resolved_argv);
			return 0;
		}
		for (int stream = 0; stream < 3; ++stream)
		{
			int source = source_fds[stream];
			if (source >= 0 && (source < 3 || source - 3 >= FS_MAX_FDS || current_process->fds[source - 3].mode == FILE_NOT_CREATED))
			{
				kfree(resolved_argv);
				return 0;
			}
		}
	}

	if (a2)
		current_process->terminal_exec = 1;
	t_process *proc = elf_to_proc((char *)elf_path, resolved_argv, argv_len);
	if (a2)
		current_process->terminal_exec = 0;
	if (!proc)
	{
		log("\033[31mSYS_EXEC\033[0m: Could not start process  '%s'...\n", LOG_WARNING | LOG_INDENT, elf_path);
		return 0;
	}
	if (a2)
	{
		for (int stream = 0; stream < 3; ++stream)
		{
			int source = source_fds[stream];
			if (source < 0)
				continue;
			proc->stdio_fds[stream] = current_process->fds[source - 3];
			proc->stdio_redirected[stream] = 1;
		}
	}
	kfree(resolved_argv);
	return proc->pid;
}


int	sys_kill(uint32_t pid, uint32_t code, uint32_t a2, uint32_t a3)
{
	(void)a2;
	(void)a3;
	t_process *proc = scheduled_processes;
	if (current_process->pid == pid)
	{
		process_exit_current(code);
		return 1;
	}
	while (proc)
	{
		if (proc->pid == pid)
		{
			if (proc->mode == PROCESS_KERNEL && current_process->mode != PROCESS_KERNEL)
				return 0;

			t_process *exiting = proc;
			if (exiting->surface_addr)
			{
				kfree((void *)(uintptr_t)exiting->surface_snapshot);
				exiting->surface_snapshot = 0;
				uint32_t surface_addr = exiting->surface_addr;
				exiting->surface_addr = 0;
				kfree((void *)(uintptr_t)surface_addr);
			}

			// cleanup opened files
			for (int i = 0; i < FS_MAX_FDS; i++)
			{
				if (exiting->fds[i].file.first_cluster)
					fat32_close(&exiting->fds[i].file);
			}

			log("Killed '%s' with status code: %i\n", LOG_WARNING, exiting->proc_name, code);

			exiting->state = PROC_ZOMBIE;
			scheduler_remove(exiting);
			
			return 1;
		}
		proc = proc->next;
	}
	return 0;
}

int	sys_pstate(uint32_t pid, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	t_process *proc = scheduled_processes;
	while (proc)
	{
		if (proc->pid == pid)
			return (uint32_t)proc->state;
		proc = proc->next;
	}
	return PROC_ZOMBIE;
}

int	sys_evtpop(uint32_t event_ptr, uint32_t a1, uint32_t a2, uint32_t a3)
{
	t_process *event_owner;
	(void)a1;
	(void)a2;
	(void)a3;
	if (!event_ptr)
		return -1;

	event_t	*event = (event_t *)resolve_user_ptr(event_ptr, sizeof(event_t));
	if (!event)
		return -1;
	*event = (event_t){0};
	event_owner = thread_memory_owner(current_process);
	return fifo_pop(&event_owner->events, event);
}

static int	thread_owned_by_current(t_process *thread)
{
	uint32_t owner_pid;
	if (!thread || !current_process || !thread->is_thread)
		return 0;
	owner_pid = current_process->is_thread ? current_process->thread_owner_pid : current_process->pid;
	return thread->thread_owner_pid == owner_pid;
}

static t_process	*find_thread(uint32_t pid)
{
	for (t_process *thread = scheduled_processes; thread; thread = thread->next)
		if (thread->pid == pid && thread_owned_by_current(thread))
			return thread;
	return 0;
}

int sys_thread_create(uint32_t function, uint32_t trampoline, uint32_t a2, uint32_t a3)
{
	uint64_t image_end;
	(void)a2;
	(void)a3;
	if (!current_process || current_process->mode != PROCESS_USER || !function || !trampoline)
		return -1;

	image_end = (uint64_t)current_process->image_vaddr_base + current_process->image_size;
	if (function < current_process->image_vaddr_base || function >= image_end || trampoline < current_process->image_vaddr_base || trampoline >= image_end)
		return -1;

	t_process *thread = thread_create_current((void (*)(void))(uintptr_t)function, (void (*)(void))(uintptr_t)trampoline);
	return thread ? (int)thread->pid : -1;
}

int sys_thread_join(uint32_t pid, uint32_t a1, uint32_t a2, uint32_t a3)
{
	t_process *thread = find_thread(pid);
	(void)a1;
	(void)a2;
	(void)a3;
	if (!thread || thread->pid == current_process->pid)
		return -1;

	return thread->state == PROC_ZOMBIE || thread->state == PROC_CRASHED;
}

int sys_thread_destroy(uint32_t pid, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a1;
	(void)a2;
	(void)a3;
	return thread_destroy_current(pid);
}

int sys_thread_kill(uint32_t pid, uint32_t a1, uint32_t a2, uint32_t a3)
{
	t_process *thread = find_thread(pid);
	(void)a1;
	(void)a2;
	(void)a3;
	if (!thread || thread->state == PROC_ZOMBIE || thread->state == PROC_CRASHED)
		return -1;
	thread->state = PROC_CRASHED;
	return thread == current_process ? 2 : 0;
}

int sys_thread_exit(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	(void)a0;
	(void)a1;
	(void)a2;
	(void)a3;
	if (!current_process || !current_process->is_thread)
		return -1;
	current_process->state = PROC_ZOMBIE;
	return 2;
}

int sys_terminal_input(uint32_t pid, uint32_t key, uint32_t a2, uint32_t a3)
{
	(void)a2;
	(void)a3;
	if (!current_process)
		return -1;
	for (t_process *proc = scheduled_processes; proc; proc = proc->next)
	{
		if (proc->pid != pid || proc->parent_pid != current_process->pid || proc->state == PROC_ZOMBIE || proc->state == PROC_CRASHED)
			continue;
		uint32_t irq_state = disable_interrupts();
		if (proc->terminal_input_count >= sizeof(proc->terminal_input))
		{
			restore_interrupts(irq_state);
			return -1;
		}
		proc->terminal_input[proc->terminal_input_tail] = (uint8_t)key;
		proc->terminal_input_tail = (proc->terminal_input_tail + 1) % sizeof(proc->terminal_input);
		proc->terminal_input_count++;
		restore_interrupts(irq_state);
		return 0;
	}
	return -1;
}


syscall_t	syscall_table[] = {
	sys_exit,
	sys_read,
	sys_write,
	sys_open,
	sys_close,
	sys_lseek,
	sys_uname,
	sys_getdents,
	sys_rename,
	sys_mkdir,
	sys_rmdir,
	sys_remove,
	sys_sbrk,
	sys_surface_create,
	sys_surface_update,
	sys_get_time,
	sys_random,
	sys_exec,
	sys_kill,
	sys_pstate,
	sys_evtpop,
	sys_thread_create,
	sys_thread_join,
	sys_thread_destroy,
	sys_thread_kill,
	sys_thread_exit,
	sys_terminal_input
};

// TODO: kill, waitpid

int	syscall_dispatch(uint32_t number, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	if (number >= sizeof(syscall_table)/sizeof(syscall_table[0]))
		return -1;
	return syscall_table[number](a0, a1, a2, a3);
}

int	syscall_handler(syscall_frame_t *frame)
{
	uint32_t cpsr = disable_interrupts();
	mmu_switch_table(mmu_kernel_table());
	int ret = syscall_dispatch(frame->r7, frame->r0, frame->r1, frame->r2, frame->r3);
	if (frame->r7 == SYSCALL_EXIT_INDEX)
	{
		restore_interrupts(cpsr);
		return 1;
	}
	if (frame->r7 == SYSCALL_THREAD_EXIT_INDEX || (frame->r7 == SYSCALL_THREAD_KILL_INDEX && ret == 2))
	{
		restore_interrupts(cpsr);
		return 2;
	}

	frame->r0 = ret;
	mmu_switch_current();
	restore_interrupts(cpsr);
	return 0;
}
