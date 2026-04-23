#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "types.h"

#define USB_BASE 0x3F980000


#define GAHBCFG   (*(volatile uint32_t*)(USB_BASE + 0x008))
#define GUSBCFG   (*(volatile uint32_t*)(USB_BASE + 0x00C))
#define GRSTCTL   (*(volatile uint32_t*)(USB_BASE + 0x010))
#define GINTSTS   (*(volatile uint32_t*)(USB_BASE + 0x014))
#define GINTMSK   (*(volatile uint32_t*)(USB_BASE + 0x018))

#define HCFG      (*(volatile uint32_t*)(USB_BASE + 0x400))
#define HFIR      (*(volatile uint32_t*)(USB_BASE + 0x404))
#define HPRT      (*(volatile uint32_t*)(USB_BASE + 0x440))



extern int					cursor_x;
extern int					cursor_y;

void	virtio_mouse_poll();
int		virtio_mouse_init(uint64_t base);
int		virtio_is_input(uint64_t base);

int		usb_device_connected();

#endif