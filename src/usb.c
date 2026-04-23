#include "peripheral.h"

void	dwc2_init()
{
	// core soft reset
	GRSTCTL |= (1 << 0);
	while (GRSTCTL & (1 << 0));

	// force host mode
	GUSBCFG |= (1 << 29);

	// enable global interrupts
	GAHBCFG |= 1;

	// host config
	HCFG |= 1; // FS/LS PHY

	// power port
	HPRT |= (1 << 12);
}

int		usb_device_connected()
{
	return (HPRT & (1 << 0));
}

void	usb_port_reset()
{
	HPRT |= (1 << 8);
	for (volatile int i = 0; i < 1000000; i++);
	HPRT &= ~(1 << 8);
}