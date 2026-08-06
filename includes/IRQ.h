#ifndef IRQ_H
#define IRQ_H

#include "types.h"

#define ST_BASE					0x20003000
#define ST_CS					(*(volatile uint32_t *)(ST_BASE + 0x00))
#define ST_CLO					(*(volatile uint32_t *)(ST_BASE + 0x04))
#define ST_CHI					(*(volatile uint32_t *)(ST_BASE + 0x08))
#define ST_C0					(*(volatile uint32_t *)(ST_BASE + 0x0C))
#define ST_C1					(*(volatile uint32_t *)(ST_BASE + 0x10))
#define ST_C2					(*(volatile uint32_t *)(ST_BASE + 0x14))
#define ST_C3					(*(volatile uint32_t *)(ST_BASE + 0x18))

#define IRQ_BASE				0x2000B200
#define IRQ_PENDING_BASIC		(*(volatile uint32_t *)(IRQ_BASE + 0x00))
#define IRQ_PENDING_1			(*(volatile uint32_t *)(IRQ_BASE + 0x04))
#define IRQ_PENDING_2			(*(volatile uint32_t *)(IRQ_BASE + 0x08))

#define IRQ_ENABLE_BASIC		(*(volatile uint32_t *)(IRQ_BASE + 0x18))
#define IRQ_ENABLE_1			(*(volatile uint32_t *)(IRQ_BASE + 0x10))
#define IRQ_ENABLE_2			(*(volatile uint32_t *)(IRQ_BASE + 0x14))

#define IRQ_DISABLE_1			(*(volatile uint32_t *)(IRQ_BASE + 0x1C))
#define IRQ_DISABLE_2			(*(volatile uint32_t *)(IRQ_BASE + 0x20))
#define SYSTEM_TIMER_1_IRQ		(1 << 1)


#define TIMER_TICK_US 1000


void timer_init(void);
void timer_ack(void);

void irq_dispatch(void);

extern void enable_irq(void);
extern void disable_irq(void);

#endif