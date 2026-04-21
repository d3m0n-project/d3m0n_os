#ifndef TIME_H
#define TIME_H

#include "types.h"

uint64_t	time_now();
uint64_t    time_us();
void		sleep(uint64_t seconds);
void		usleep(uint64_t micro_seconds);

#endif