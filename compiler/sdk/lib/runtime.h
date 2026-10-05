#ifndef RUNTIME_H
#define RUNTIME_H

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

	extern int		_setjmp(jmp_buf env);
	extern int		_longjmp(jmp_buf env, int value);
	static inline int setjmp(jmp_buf env) { return _setjmp(env); }
	static inline int longjmp(jmp_buf env, int val) { return _longjmp(env, val); }

	void			abort(void);

	extern int		errno;
	const char		*strerror(int errnb);
#ifdef __cplusplus
}
#endif

#endif