#include "protocol/mailbox.h"
#include "memory/mmu.h"

framebuffer_req_t	fb_req = {0};
volatile uint32_t *mailbox = (volatile uint32_t *)MAILBOX_BASE;

void	mailbox_write(uint32_t value, uint8_t channel)
{
	while(mailbox[MAILBOX_STATUS/4] & MAILBOX_FULL);
	mailbox[MAILBOX_WRITE/4] = (value & 0xFFFFFFF0) | (channel & 0xF);
}

uint32_t	mailbox_read(uint8_t channel)
{
	uint32_t r;
	do {
		while(mailbox[MAILBOX_STATUS/4] & MAILBOX_EMPTY);
		r = mailbox[MAILBOX_READ/4];
	} while((r & 0xF) != channel);
	return r & 0xFFFFFFF0;
}

int	mailbox_call(volatile uint32_t *message, uint8_t channel)
{
	uintptr_t	address;

	address = (uintptr_t)message;
	/* The message buffer lives in cacheable RAM and is read by the GPU (DMA).
	   Write back + invalidate before handing it over so the GPU sees the data. */
	mmu_clean_invalidate_dcache_range((uint32_t)address, 35 * sizeof(uint32_t));
	mailbox_write((uint32_t)address, channel);
	if (mailbox_read(channel) != (uint32_t)address)
		return (0);
	/* The GPU wrote the response into the same buffer; pull it out of D-cache. */
	mmu_clean_invalidate_dcache_range((uint32_t)address, 35 * sizeof(uint32_t));
	return (message[1] == 0x80000000);
}