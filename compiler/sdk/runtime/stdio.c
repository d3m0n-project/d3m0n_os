#include "stdio.hpp"

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f)
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

FILE *fopen(const char *path, const char *mode)
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

	f->buf = 0;
	f->buf_size = 0;
	f->buf_pos = 0;
	f->buf_len = 0;
	f->buf_mode = _IONBF;
	f->buf_owned = 0;

	f->fd = fd;
	f->ungot = 0;
	f->is_eof = 0;
	f->error = 0;

	return f;
}

int	fputs(const char *string, FILE *stream)
{
	size_t	len = strlen(string);
	write(stream->fd, string, len);
	return 0;
}

int	fputc(int character, FILE *stream)
{
	char c = character;
	write(stream->fd, &c, 1);
	return 0;
}

FILE		*freopen(const char *path, const char *mode, FILE *f)
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

	f->buf = 0;
	f->buf_size = 0;
	f->buf_pos = 0;
	f->buf_len = 0;
	f->buf_mode = _IONBF;
	f->buf_owned = 0;

	f->fd = fd;
	f->ungot = 0;
	f->is_eof = 0;
	f->error = 0;
	return f;
}

int	getc(FILE *f)
{
	int	c;
	if (f->ungot > 0)
	{
		c = f->ungot;
		f->ungot = 0;
		return c;
	}

	c = EOF;
	if (fread(&c, 1, 1, f) != 1)
		return EOF;

	return c;
}


int	ferror(FILE *f)
{
	return f->error;
}

int	fclose(FILE *f)
{
	int	ret;

	if (f == 0)
		return EOF;

	if (f->buf_owned && f->buf)
		free(f->buf);

	ret = close(f->fd);
	if (ret < 0)
	{
		f->error = 1;
		return EOF;
	}
	free(f);
	return 0;
}

int	fflush(FILE *f)
{
	// ret EOF if err

	// clear buffered
	if (f->ungot > 0)
		f->ungot = 0;
	// TODO: real flushing ? maybe
	return 0;
}

size_t	fwrite(const char *buff, size_t size, size_t nmemb, FILE *f)
{
	return write(f->fd, buff, size * nmemb);
}

char	*fgets(char *string, int size, FILE *f)
{
	if (f->ungot > 0)
		return (char *)&f->ungot;
	(void)size;
	fread(string, 1, size, f); // TODO: check this
	return string;
}

FILE	*tmpfile()
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

char	*tmpnam()
{
	char	*ret = (char *)calloc(L_tmpnam, sizeof(char));
	if (!ret)
		return 0;
	
	// TODO: retry if error
	rng_bytes(ret, L_tmpnam - 5); // - '.tmp'
	int i = 0;
	for (; i<L_tmpnam - 5; i++)
		ret[i] = 'A' + (ret[i] % 57);

	ret[i++] = '.';
	ret[i++] = 't';
	ret[i++] = 'm';
	ret[i++] = 'p';
	ret[i++] = '\0';

	return ret;
}

int	ungetc(int c, FILE *f)
{
	if (c <= 0)
		return 0;
	f->ungot = c;
	return c;
}

void	clearerr(FILE *f)
{
	f->error = 0;
	f->is_eof = 0;
}

int	fseek(FILE *f, long offset, e_seek_directive whence)
{
	long	ret;
	if (!f)
		return -1;

	ret = lseek(f->fd, offset, whence);
	if (ret < 0)
	{
		f->error = 1;
		return -1;
	}

	f->ungot = 0;
	f->is_eof = 0;

	return 0;
}


long	ftell(FILE *f)
{
	long pos;
	if (!f)
		return -1;

	pos = lseek(f->fd, 0, SEEK_CUR);
	if (pos < 0)
	{
		f->error = 1;
		return -1;
	}

	if (f->ungot > 0)
		pos--;

	return pos;
}

int	setvbuf(FILE *f, char *buf, int mode, size_t size)
{
	char	*new_buf;
	if (!f)
		return -1;

	if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF)
	{
		f->error = 1;
		return -1;
	}


	if (f->buf_owned && f->buf)
		free(f->buf);

	f->buf = 0;
	f->buf_size = 0;
	f->buf_pos = 0;
	f->buf_len = 0;
	f->buf_owned = 0;
	f->buf_mode = mode;

	if (mode == _IONBF)
		return 0;

	if (size == 0)
	{
		f->error = 1;
		return -1;
	}

	if (buf)
	{
		f->buf = buf;
		f->buf_size = size;
	}
	else
	{
		new_buf = (char *)malloc(size);
		if (!new_buf)
		{
			f->error = 1;
			return -1;
		}

		f->buf = new_buf;
		f->buf_size = size;
		f->buf_owned = 1;
	}
	return 0;
}