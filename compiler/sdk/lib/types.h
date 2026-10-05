#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
#include "exception"

using namespace std;

extern "C" {
#endif
	/** @brief Stores the s_point data. */
	typedef struct s_point
	{
		int	x;
		int	y;
	}	t_point;

	typedef unsigned char			uint8_t;
	typedef unsigned short			uint16_t;
	typedef unsigned int			uint32_t;
	typedef unsigned long long		uint64_t;

	typedef signed char				int8_t;
	typedef signed short			int16_t;
	typedef signed int				int32_t;
	typedef signed long long		int64_t;
	typedef unsigned int			uintptr_t;

	typedef unsigned int			size_t;
	typedef int						ssize_t;

	#define SIZE_MAX				(size_t)-1

	#define	OFFSETOF(TYPE, ELEMENT)	((size_t)&(((TYPE *)0)->ELEMENT))

	typedef __builtin_va_list		va_list;
	#define va_start(ap,last)		__builtin_va_start(ap,last)
	#define va_arg(ap,type)			__builtin_va_arg(ap,type)
	#define va_copy(dst,src)		__builtin_va_copy(dst,src)
	#define va_end(ap)				__builtin_va_end(ap)
#ifdef __cplusplus
}
/** @brief Represents the AppException type. */
class AppException : public exception
{
private:
	const char *value;
public:
	AppException(const char *val) {
		this->value = val;
	}

	/** @brief what operation. */
	const char* what() {
		return this->value;
	}
};

#define assert(cond, ret)	if (!(cond)) {printf("ASSERT ERROR: %s\n", #cond); return ret;}
#else
#define NULL	((void *)0)
#define assert(cond, ret)	if (!(cond)) {log("ASSERT ERROR: %s\n", LOG_WARNING, #cond); return ret;}
#endif

#endif