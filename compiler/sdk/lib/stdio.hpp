#ifndef STDIO_HPP
#define STDIO_HPP

#include "types.h"
#include "runtime.h"
#include "sys.h"
#include "printf/printf.hpp"
#include "stdlib.h"
#include "memory.h"
#include "app/app_manifest.h"

#define BUFSIZ	1024

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		int		fd;
		int		is_eof;
		int		error;
		int		ungot;
	}	FILE;

	#define EOF		-1
	#define stderr	(&(FILE){.fd = 2, .is_eof = 0, .error = 0, .ungot=0})
	#define stdout	(&(FILE){.fd = 1, .is_eof = 0, .error = 0, .ungot=0})
	#define stdin	(&(FILE){.fd = 0, .is_eof = 0, .error = 0, .ungot=0}) // TODO: stdin

	int					sprintf(char *str, const char *format, ...);
	int					snprintf(char *str, size_t size, const char *format, ...);
	size_t				printf(const char *format, ...);
	size_t				fprintf(FILE *f, const char *format, ...);

	static inline int	feof(FILE *f) { return f->is_eof; }
	static inline size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f)
	{
		size_t	total;
		size_t	done;
		ssize_t	ret;
		if (size == 0 || nmemb == 0)
			return 0;

		if (f->is_eof)
			return 0;

		total = size * nmemb;
		done = 0;
		while (done < total)
		{
			ret = read(f->fd, (char *)ptr + done, total - done);
			if (ret < 0)
			{
				f->error = 1;
				break;
			}

			if (ret == 0)
			{
				f->is_eof = 1;
				break;
			}
			done += (size_t)ret;
		}
		return done / size;
	}

	static inline FILE *fopen(const char *path, const char *mode)
	{
		int		fd;
		int		flags;
		FILE	*f;

		if (!path || !mode || !*mode)
			return 0;

		switch (*mode)
		{
			case 'r':
				flags = O_READ;
				break;
			case 'w':
				flags = O_WRITE | O_CREATE | O_TRUNC;
				break;
			case 'a':
				flags = O_WRITE | O_CREATE | O_APPEND;
				break;
			default:
				return 0;
		}

		fd = open(path, flags);
		if (fd < 0)
			return 0;

		f = (FILE *)malloc(sizeof(FILE));
		if (!f)
		{
			close(fd);
			return 0;
		}

		f->fd = fd;
		f->ungot = 0;
		f->is_eof = 0;
		f->error = 0;

		return f;
	}

	static inline FILE	*freopen(const char *path, const char *mode, FILE *f)
	{
		int	fd;
		int	flags;

		if (!path || !mode || !f)
			return 0;

		switch (*mode)
		{
			case 'r':
				flags = O_READ;
				break;
			case 'w':
				flags = O_WRITE | O_CREATE | O_TRUNC;
				break;
			case 'a':
				flags = O_WRITE | O_CREATE | O_APPEND;
				break;
			default:
				f->error = 1;
				return 0;
		}

		if (close(f->fd) < 0)
		{
			f->error = 1;
			return 0;
		}

		fd = open(path, flags);
		if (fd < 0)
		{
			f->error = 1;
			f->is_eof = 0;
			return 0;
		}

		f->fd = fd;
		f->ungot = 0;
		f->is_eof = 0;
		f->error = 0;
		return f;
	}

	static inline char	getc(FILE *f)
	{
		if (f->ungot > 0)
		{
			int tmp = f->ungot;
			f->ungot = 0;
			return tmp;
		}
		char c = '\0';
		fread(&c, 1, 1, f);
		return c;
	}

	static inline char	ferror(FILE *f)
	{
		return f->error;
	}

	static inline int fclose(FILE *f)
	{
		int	ret;

		if (f == 0)
			return EOF;

		ret = close(f->fd);
		if (ret < 0)
		{
			f->error = 1;
			return EOF;
		}
		free(f);
		return 0;
	}

	static inline void	fflush(FILE *f)
	{
		(void)f; // TODO: flushing ? maybe
	}

	static inline void	fwrite(char *buff, size_t size, size_t nmemb, FILE *f)
	{
		write(f->fd, buff, size * nmemb);
	}

	static inline char	*fgets(char *string, int size, FILE *f)
	{
		if (f->ungot > 0)
		{
			int tmp = f->ungot;
			f->ungot = 0;
			return tmp;
		}
		fread(string, size, 1, f); // TODO: check this
		return string;
	}

	static inline FILE	*tmpfile()
	{
		FILE	*ret = (FILE *)malloc(sizeof(FILE));
		if (!ret)
			return 0;
		
		char tmp_name[] = "/tmp/[........].tmp"; // TODO: retry if error
		rng_bytes(tmp_name + 6, 10);
		for (int i=6; i<10 + 6; i++)
			tmp_name[i] = 'A' + (tmp_name[i] % 57);
		ret->fd = open(tmp_name, O_CREATE | O_WRITE);
		if (ret->fd < 0)
			return 0;
		
		ret->error = 0;
		ret->is_eof = 0;
		return ret;
	}
#ifdef __cplusplus
}
#endif

#endif