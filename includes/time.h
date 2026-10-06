#ifndef TIME_H
#define TIME_H

#include "types.h"

uint64_t	time_from_boot(void);
uint64_t    time_us(void);
uint64_t    time_s(void);
void		sleep(uint64_t seconds);
void		usleep(uint64_t micro_seconds);
void		set_time(uint64_t time);

#endif