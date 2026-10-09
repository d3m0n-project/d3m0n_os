#ifndef STDIO_HPP
#define STDIO_HPP

#include "types.h"
#include "runtime.h"
#include "sys.h"
#include "printf/printf.hpp"
#include "stdlib.h"
#include "memory.h"
#include "app/app_manifest.h"

#define BUFSIZ			1024
#define _IOFBF			0	// fully buffered
#define _IOLBF			1	// line buffered
#define _IONBF			2	// unbuffered

#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1

#define L_tmpnam		10

#if L_tmpnam <= 5
	#error "L_tmpnam must be greater than 5 to ensure valid tmp names"
#endif

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		int		fd;
		int		is_eof;
		int		error;
		int		ungot;
		char	*buf;
		size_t	buf_size;
		size_t	buf_pos;
		size_t	buf_len;
		int		buf_mode;
		int		buf_owned;
	} FILE;

	#define EOF		-1
	#define stderr	(&(FILE){.fd = 2, .is_eof = 0, .error = 0, .ungot=0})
	#define stdout	(&(FILE){.fd = 1, .is_eof = 0, .error = 0, .ungot=0})
	#define stdin	(&(FILE){.fd = 0, .is_eof = 0, .error = 0, .ungot=0}) // TODO: stdin

	int					sprintf(char *str, const char *format, ...);
	int					snprintf(char *str, size_t size, const char *format, ...);
	size_t				printf(const char *format, ...);
	size_t				fprintf(FILE *f, const char *format, ...);

	static inline int	feof(FILE *f) { return f->is_eof; }

	size_t				fread(void *ptr, size_t size, size_t nmemb, FILE *f);
	FILE				*fopen(const char *path, const char *mode);
	int					fputs(const char *string, FILE *stream);
	int					fputc(int character, FILE *stream);
	FILE				*freopen(const char *path, const char *mode, FILE *f);
	int					getc(FILE *f);
	int					ferror(FILE *f);
	int					fclose(FILE *f);
	int					fflush(FILE *f);
	size_t				fwrite(const char *buff, size_t size, size_t nmemb, FILE *f);
	char				*fgets(char *string, int size, FILE *f);
	FILE				*tmpfile();
	char				*tmpnam();
	int					ungetc(int c, FILE *f);
	void				clearerr(FILE *f);
	int					fseek(FILE *f, long offset, e_seek_directive whence);
	long				ftell(FILE *f);
	int					setvbuf(FILE *f, char *buf, int mode, size_t size);
#ifdef __cplusplus
}
#endif

#endif