#ifndef TYPES_H
#define TYPES_H

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed int			int32_t;
typedef signed long long	int64_t;

#define NULL ((void *)0)

typedef __builtin_va_list	va_list;
#define va_start(ap,last)	__builtin_va_start(ap,last)
#define va_arg(ap,type)		__builtin_va_arg(ap,type)
#define va_end(ap)			__builtin_va_end(ap)

#endif