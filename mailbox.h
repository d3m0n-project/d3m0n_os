#ifndef MAILBOX_H
#define MAILBOX_H

#include "peripheral.h"
#include "types.h"

#define MAILBOX_BASE	(PERIPHERAL_BASE + 0xB880)
#define MAILBOX_READ	0x0
#define MAILBOX_STATUS	0x18
#define MAILBOX_WRITE	0x20

#define MAILBOX_FULL  0x80000000
#define MAILBOX_EMPTY 0x40000000

#define MAILBOX_REQUEST               0x00000000
#define MAILBOX_RESPONSE_SUCCESS      0x80000000
#define MAILBOX_TAG_SET_PHYS_SIZE     0x00048003
#define MAILBOX_TAG_SET_VIRT_SIZE     0x00048004
#define MAILBOX_TAG_SET_VIRT_OFFSET   0x00048009
#define MAILBOX_TAG_SET_DEPTH         0x00048005
#define MAILBOX_TAG_SET_PIXEL_ORDER   0x00048006
#define MAILBOX_TAG_ALLOCATE_BUFFER   0x00040001
#define MAILBOX_TAG_GET_PITCH         0x00040008
#define MAILBOX_TAG_LAST              0x00000000

typedef struct s_framebuffer_req {
    uint32_t width;
    uint32_t height;
    uint32_t virt_width;
    uint32_t virt_height;
    uint32_t pitch;
    uint32_t depth;
    uint32_t x_offset;
    uint32_t y_offset;
    uintptr_t fb_addr;
    uint32_t fb_size;
} framebuffer_req_t;

extern framebuffer_req_t	fb_req;
extern volatile uint32_t	*mailbox;

void		mailbox_write(uint32_t value, uint8_t channel);
uint32_t	mailbox_read(uint8_t channel);
int		mailbox_call(volatile uint32_t *message, uint8_t channel);

#define MAILBOX_CH_PROP 8

#endif