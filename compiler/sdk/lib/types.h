#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
#include "exception"

using namespace std;

extern "C" {
#endif
	#ifdef KERNEL_VERSION
		extern void			log(const char *fmt, int type, ...);
	#else
		extern int			printf(const char *fmt, ...);
	#endif

	/** @brief Stores a two-dimensional integer coordinate. */
	typedef struct s_point
	{
		int	x;
		int	y;
	}	t_point;

	#define CLOCKS_PER_SEC			1000000U

	#define bool					int
	#define TRUE					1
	#define FALSE					0
	#define true					1
	#define false					0

	typedef unsigned char			uint8_t;
	typedef unsigned short			uint16_t;
	typedef unsigned int			uint32_t;
	typedef unsigned long long		uint64_t;

	//typedef unsigned int			wchar_t;

	typedef signed char				int8_t;
	typedef signed short			int16_t;
	typedef signed int				int32_t;
	typedef signed long long		int64_t;
	typedef unsigned int			uintptr_t;
	typedef int						intptr_t;
	typedef int						ptrdiff_t;

	typedef int						sig_atomic_t;

	typedef unsigned int			size_t;
	typedef int						ssize_t;

	typedef long long unsigned int	uintmax_t;
	typedef long long int			intmax_t;

	typedef long					off_t;
	typedef unsigned long			ino_t;

	
	#define UINT8_MAX				0xFFU
	#define UINT16_MAX				0xFFFFU
	#define UINT32_MAX				0xFFFFFFFFU
	#define UINT64_MAX				0xFFFFFFFFFFFFFFFFU
	#define SIZE_MAX				UINT32_MAX
	#define UINT_MAX				UINT32_MAX
	#define LLONG_MAX				0x7FFFFFFFFFFFFFFF
	#define LLONG_MIN				0x8000000000000000
	#define	ULLONG_MAX				UINT64_MAX
	#define INT_MAX					2147483647
	#define INT_MIN					-2147483648
	#define INTPTR_MAX				0x7FFFFFFF
	#define INTPTR_MIN				0x80000000
	#define SIZEOF_VOID_P			4
	#define UINTPTR_MAX				0xFFFFFFFFU
	

	#define CHAR_BIT				8
	#define UCHAR_MAX				255
	#define CHAR_MAX				127
	#define CHAR_MIN				(-128)

	#define USHRT_MAX 				65535
	#define SHRT_MAX				32767
	#define SHRT_MIN				(-32768)

	// float limits
	#define FLT_MANT_DIG			24
	#define FLT_DIG					6
	#define FLT_MIN_EXP				(-125)
	#define FLT_MAX_EXP				128
	#define FLT_MIN_10_EXP			(-37)
	#define FLT_MAX_10_EXP			38


	#define	OFFSETOF(TYPE, ELEMENT)	((size_t)&(((TYPE *)0)->ELEMENT))

	typedef __builtin_va_list		va_list;
	#define va_start(ap,last)		__builtin_va_start(ap,last)
	#define va_arg(ap,type)			__builtin_va_arg(ap,type)
	#define va_copy(dst,src)		__builtin_va_copy(dst,src)
	#define va_end(ap)				__builtin_va_end(ap)

	typedef int32_t					time_t;

	static inline void	assert_fail(const char *condition)
	{
		#ifdef KERNEL_VERSION
			log("ASSERT ERROR: %s\n", 4, condition); // warn
		#else
			printf("ASSERT ERROR: %s\n", condition);
		#endif

		while (1)
			__asm__ volatile("wfe");
	}

#ifdef __cplusplus
}
/** @brief Carries an error message for application initialization failures. */
class AppException : public exception
{
private:
	const char *value;
public:
	AppException(const char *val) {
		this->value = val;
	}

	/** @brief Returns the exception message. */
	const char* what() {
		return this->value;
	}
};

#else
	#define NULL				((void *)0)
#endif

#define assert(cond)			((cond)?(void)0:assert_fail(#cond))

#endif