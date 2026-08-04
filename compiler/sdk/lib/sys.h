#ifndef SYS_H
#define SYS_H

#include "../../../includes/types.h"

typedef enum
{
    SYS_EXIT,
    SYS_PRINT,
}   e_syscall_indexes;

static inline int	syscall(uint32_t number, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    register uint32_t r0 asm("r0") = a0;
    register uint32_t r1 asm("r1") = a1;
    register uint32_t r2 asm("r2") = a2;
    register uint32_t r3 asm("r3") = a3;
    register uint32_t r7 asm("r7") = number;

    asm volatile(
        "svc #0"
        : "+r"(r0)
        : "r"(r1),
          "r"(r2),
          "r"(r3),
          "r"(r7)
        : "memory"
    );
    return (int)r0;
}

static inline void    exit(int ret_code)
{
    syscall(SYS_EXIT, (uint32_t)ret_code, 0, 0, 0);
}

static inline void    print(char *text)
{
    syscall(SYS_PRINT, (uint32_t)text, 0, 0, 0);
}

#endif