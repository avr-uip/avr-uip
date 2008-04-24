#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

#include <stdint.h>

typedef uint32_t clock_time_t;

//TODO: This should be calculate based on Fcpu, prescaler and software prescaler in clock-arch.h
#define CLOCK_CONF_SECOND 24

void clock_init(void);
clock_time_t clock_time(void);

#endif /* __CLOCK_ARCH_H__ */
