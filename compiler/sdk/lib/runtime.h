#ifndef RUNTIME_H
#define RUNTIME_H

#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct
	{
		unsigned int r4;
		unsigned int r5;
		unsigned int r6;
		unsigned int r7;
		unsigned int r8;
		unsigned int r9;
		unsigned int r10;
		unsigned int r11;
		unsigned int sp;
		unsigned int lr;
	}	jmp_buf[1];

	extern int			_setjmp(jmp_buf env);
	extern int			_longjmp(jmp_buf env, int value);
	static inline int	setjmp(jmp_buf env) { return _setjmp(env); }
	static inline int	longjmp(jmp_buf env, int val) { return _longjmp(env, val); }

	void				abort(void);

	extern int			__errno;
	extern int			errno;
	const char			*strerror(int errnb);

	static inline char	*rng_bytes(char *buff, size_t size)
	{
		for (size_t i=0; i<size; i++)
		{
			buff[i] = (char)(random_u32() & 0xFF);
		}
		return buff;
	}

	void	abort(void);

	char	*getenv(const char *__name);

	// TODO: signal handling
	typedef void		(*sighandler_t)(int);
	#define SIG_DFL		((sighandler_t)0)
	#define SIG_IGN		((sighandler_t)1)
	#define SIG_ERR		((sighandler_t)-1)

	#define SIGINT		0

	static inline sighandler_t	signal(int sig, sighandler_t handler)
	{
		// TODO:
		(void)sig;
		return handler;
	}

#ifdef __cplusplus
}
#endif

#endif