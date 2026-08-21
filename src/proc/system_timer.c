#include "interrupts.h"


void timer_init(void)
{
	// clear old pending
	ST_CS = SYSTEM_TIMER_1_IRQ;


	// program first timer event
	ST_C1 = ST_CLO + TIMER_TICK_US;
	IRQ_ENABLE_1 = SYSTEM_TIMER_1_IRQ;
}



void timer_ack(void)
{
	// clear the interrupt
	ST_CS = SYSTEM_TIMER_1_IRQ;

	// schedule next tick
	ST_C1 = ST_CLO + TIMER_TICK_US;
}