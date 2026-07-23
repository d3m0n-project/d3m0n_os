#include "peripheral.h"
#include "time.h"
#include "usb.h"
#include "display.h"
#include "controls.h"

#define USB_BASE			(PERIPHERAL_BASE + 0x00980000)

#define REG32(x)			(*(volatile uint32_t *)(USB_BASE + (x)))

#define GOTGCTL				REG32(0x000)
#define GAHBCFG				REG32(0x008)
#define GUSBCFG				REG32(0x00C)
#define GRSTCTL				REG32(0x010)
#define GINTSTS				REG32(0x014)
#define GINTMSK				REG32(0x018)
#define GRXFSIZ				REG32(0x024)
#define GNPTXFSIZ			REG32(0x028)
#define HCFG				REG32(0x400)
#define HFNUM				REG32(0x408)
#define HPRT				REG32(0x440)
#define HAINTMSK			REG32(0x418)

#define HC(n)				(*(usb_hc_t *)(USB_BASE + 0x500 + ((n) * 0x20)))

#define USB_TIMEOUT			1000000
#define USB_RETRIES			16
#define USB_EP0				0
#define USB_ADDR_FIRST		1
#define USB_ADDR_HUB		1
#define USB_ADDR_CHILD		2

#define GAHBCFG_GLBL_INTR	(1 << 0)
#define GAHBCFG_DMA_EN		(1 << 5)
#define GUSBCFG_FORCE_HOST	(1 << 29)
#define GRSTCTL_CSFTRST		(1 << 0)
#define GRSTCTL_RXFFLSH		(1 << 4)
#define GRSTCTL_TXFFLSH		(1 << 5)
#define GRSTCTL_AHBIDLE		(1 << 31)

#define HPRT_CONNSTS		(1 << 0)
#define HPRT_CONNDET		(1 << 1)
#define HPRT_ENA			(1 << 2)
#define HPRT_ENACHG			(1 << 3)
#define HPRT_OVRCURRCHG		(1 << 5)
#define HPRT_PWR			(1 << 12)
#define HPRT_PRTRST			(1 << 8)
#define HPRT_SPD_MASK		(3 << 17)
#define HPRT_SPD_LOW		(2 << 17)

#define HCCHAR_MPS(x)		((x) & 0x7ff)
#define HCCHAR_EPNUM(x)		(((uint32_t)(x) & 0xf) << 11)
#define HCCHAR_EPDIR_IN		(1 << 15)
#define HCCHAR_LOW_SPEED	(1 << 17)
#define HCCHAR_EPTYPE_CTRL	(0 << 18)
#define HCCHAR_EPTYPE_INTR	(3 << 18)
#define HCCHAR_ODDFRM		(1 << 29)
#define HCCHAR_DEVADDR(x)	(((uint32_t)(x) & 0x7f) << 22)
#define HCCHAR_CHDIS		(1 << 30)
#define HCCHAR_CHENA		(1 << 31)

#define HCTSIZ_PID_DATA0	(0u << 29)
#define HCTSIZ_PID_DATA1	(2u << 29)
#define HCTSIZ_PID_SETUP	(3u << 29)
#define HCTSIZ_PKTCNT(x)	(((uint32_t)(x) & 0x3ff) << 19)
#define HCTSIZ_XFERSIZE(x)	((uint32_t)(x) & 0x7ffff)

#define HCINT_XFERCOMP		(1 << 0)
#define HCINT_CHHLTD		(1 << 1)
#define HCINT_STALL			(1 << 3)
#define HCINT_NAK			(1 << 4)
#define HCINT_ACK			(1 << 5)
#define HCINT_NYET			(1 << 6)
#define HCINT_TXERR			(1 << 7)
#define HCINT_BBLERR		(1 << 8)
#define HCINT_FRMOVRUN		(1 << 9)
#define HCINT_DATATGLERR	(1 << 10)
#define HCINT_ALL			0x7ff

#define USB_REQ_GET_DESCRIPTOR	6
#define USB_REQ_SET_ADDRESS		5
#define USB_REQ_SET_CONFIG		9
#define USB_REQ_SET_PROTOCOL	11
#define USB_REQ_SET_FEATURE		3
#define USB_REQ_GET_STATUS		0
#define USB_DESC_DEVICE			1
#define USB_DESC_CONFIG			2
#define USB_DESC_INTERFACE		4
#define USB_DESC_ENDPOINT		5
#define USB_CLASS_HUB			0x09
#define USB_CLASS_HID			0x03
#define USB_HID_BOOT			0x01
#define USB_HID_MOUSE			0x02
#define USB_ENDPOINT_IN			0x80
#define USB_ENDPOINT_XFER_MASK	0x03
#define USB_ENDPOINT_XFER_INTR	0x03
#define USB_PORT_POWER			8
#define USB_PORT_RESET			4
#define USB_PORT_CONNECTION		(1 << 0)
#define USB_PORT_ENABLE			(1 << 1)
#define USB_PORT_LOW_SPEED		(1 << 9)

typedef struct {
	uint8_t		addr;
	uint8_t		speed_low;
	uint16_t	ep0_mps;
}	usb_device_t;

typedef struct {
	uint8_t		present;
	uint8_t		interface;
	uint8_t		endpoint;
	uint8_t		interval;
	uint16_t	mps;
	int32_t		x;
	int32_t		y;
	uint32_t	last_pixel_color[CURSOR_SIZE * CURSOR_SIZE];
	int			last_pixel_saved;
	uint8_t		buttons;
}	usb_mouse_t;

static usb_device_t	g_root_device = {0, 0, 8};
static usb_mouse_t	g_mouse = {0};
static uint8_t		g_usb_buf[256]    __attribute__((aligned(4)));
static uint8_t		g_mouse_report[8] __attribute__((aligned(4)));

static uint16_t	usb_min_u16(uint16_t a, uint16_t b)
{
	if (a < b)
		return (a);
	return (b);
}

static void	usb_wait_ahb_idle(void)
{
	uint32_t	timeout;

	timeout = USB_TIMEOUT;
	while (!(GRSTCTL & GRSTCTL_AHBIDLE) && --timeout)
		continue;
}

static void	usb_flush_fifos(void)
{
	usb_wait_ahb_idle();
	GRSTCTL = GRSTCTL_RXFFLSH;
	while (GRSTCTL & GRSTCTL_RXFFLSH)
		continue;
	usb_wait_ahb_idle();
	GRSTCTL = GRSTCTL_TXFFLSH | (0x10 << 6);
	while (GRSTCTL & GRSTCTL_TXFFLSH)
		continue;
	usleep(1000);
}

static uint32_t	usb_hprt_read_clean(void)
{
	return (HPRT & ~(HPRT_CONNDET | HPRT_ENACHG | HPRT_OVRCURRCHG));
}

static void	usb_hprt_write(uint32_t value)
{
	HPRT = value & ~(HPRT_CONNDET | HPRT_ENACHG | HPRT_OVRCURRCHG);
}

static int	hc_wait_complete(usb_hc_t *hc)
{
	uint32_t	timeout;
	uint32_t	status;

	timeout = USB_TIMEOUT;
	while (!(status = hc->HCINT))
	{
		if (--timeout == 0)
			return (-1);
	}
	hc->HCINT = status;
	if (status & HCINT_XFERCOMP)
		return 0;
	if (status & HCINT_STALL)
		return -2;
	if (status & (HCINT_NAK | HCINT_NYET))
		return -3;
	if (status & (HCINT_TXERR | HCINT_BBLERR | HCINT_FRMOVRUN
			| HCINT_DATATGLERR))
		return -4;
	if (status & HCINT_CHHLTD)
		return -5;
	return -6;
}

static int	usb_port_reset(void)
{
	uint32_t	hprt;

	hprt = usb_hprt_read_clean();
	if (!(hprt & HPRT_CONNSTS))
		return -1;
	usb_hprt_write(hprt | HPRT_PWR | HPRT_PRTRST);
	usleep(60000);
	usb_hprt_write((usb_hprt_read_clean() | HPRT_PWR) & ~HPRT_PRTRST);
	usleep(100000);
	hprt = usb_hprt_read_clean();
	if (!(hprt & HPRT_ENA))
		return -2;
	g_root_device.speed_low = ((hprt & HPRT_SPD_MASK) == HPRT_SPD_LOW);
	return 0;
}

void	usb_init(void)
{
	GAHBCFG = 0;
	GUSBCFG |= GUSBCFG_FORCE_HOST;
	usleep(25000);
	usb_wait_ahb_idle();
	GRSTCTL |= GRSTCTL_CSFTRST;
	while (GRSTCTL & GRSTCTL_CSFTRST)
		continue;
	usleep(100000);
	usb_flush_fifos();
	GRXFSIZ = 256;
	GNPTXFSIZ = (256 << 16) | 256;
	HCFG = (HCFG & ~3) | 1;
	usb_hprt_write(usb_hprt_read_clean() | HPRT_PWR);
	GINTSTS = 0xffffffff;
	GINTMSK = 0;
	HAINTMSK = 1;
	GAHBCFG = GAHBCFG_DMA_EN | GAHBCFG_GLBL_INTR;
}

static int	usb_transfer_ex(uint8_t addr, uint8_t ep, uint8_t in, uint32_t ep_type, uint16_t mps, uint32_t pid, uint8_t *buf, uint16_t len)
{
	usb_hc_t	*hc;
	uint32_t	hcchar;
	uint32_t	packets;
	int			ret;
	int			try;

	hc = &HC(0);
	packets = 1;
	if (len)
		packets = (len + mps - 1) / mps;
	hcchar = HCCHAR_MPS(mps) | HCCHAR_EPNUM(ep) | ep_type
		| HCCHAR_DEVADDR(addr);
	if (in)
		hcchar |= HCCHAR_EPDIR_IN;
	if (g_root_device.speed_low)
		hcchar |= HCCHAR_LOW_SPEED;
	if (ep_type == HCCHAR_EPTYPE_INTR && (HFNUM & 1))
		hcchar |= HCCHAR_ODDFRM;
	hc->HCINTMSK = HCINT_ALL;
	for (try = 0; try < USB_RETRIES; try++)
	{
		hc->HCINT = 0xffffffff;
		hc->HCTSIZ = HCTSIZ_PKTCNT(packets) | HCTSIZ_XFERSIZE(len) | pid;
		hc->HCDMA = (uint32_t)(uintptr_t)buf;
		hc->HCCHAR = hcchar | HCCHAR_CHENA;
		ret = hc_wait_complete(hc);
		if (ret == 0)
			return 0;
		if (ret != -3)
			return ret;
		usleep(1000);
	}
	hc->HCCHAR = hcchar | HCCHAR_CHDIS | HCCHAR_CHENA;
	return (-3);
}

static int	usb_transfer(uint8_t addr, uint8_t ep, uint8_t in, uint32_t pid, uint8_t *buf, uint16_t len)
{
	return usb_transfer_ex(addr, ep, in, HCCHAR_EPTYPE_CTRL, g_root_device.ep0_mps, pid, buf, len);
}

static int	usb_control_transfer_ex(usb_device_t *dev, usb_setup_t *setup, uint8_t *buf)
{
	int			ret;
	uint8_t		addr;
	uint16_t	len;

	addr = dev->addr;
	len = setup->wLength;
	ret = usb_transfer(addr, USB_EP0, 0, HCTSIZ_PID_SETUP, (uint8_t *)setup, sizeof(usb_setup_t));
	if (ret)
		return ret;
	if (len)
	{
		if (setup->bmRequestType & 0x80)
			ret = usb_transfer(addr, USB_EP0, 1, HCTSIZ_PID_DATA1, buf, len);
		else
			ret = usb_transfer(addr, USB_EP0, 0, HCTSIZ_PID_DATA1, buf, len);
		if (ret)
			return ret;
	}
	if (setup->bmRequestType & 0x80)
		ret = usb_transfer(addr, USB_EP0, 0, HCTSIZ_PID_DATA1, NULL, 0);
	else
		ret = usb_transfer(addr, USB_EP0, 1, HCTSIZ_PID_DATA1, NULL, 0);
	return ret;
}

int	usb_control_transfer(uint8_t addr, usb_setup_t *setup, uint8_t *buf)
{
	usb_device_t	dev;

	dev = g_root_device;
	dev.addr = addr;
	return usb_control_transfer_ex(&dev, setup, buf);
}

static int	usb_get_descriptor_len(usb_device_t *dev, uint8_t type, uint8_t index, uint8_t *buf, uint16_t len)
{
	usb_setup_t	setup;

	setup.bmRequestType = 0x80;
	setup.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.wValue = ((uint16_t)type << 8) | index;
	setup.wIndex = 0;
	setup.wLength = len;
	return usb_control_transfer_ex(dev, &setup, buf);
}

int	usb_get_descriptor(uint8_t addr, uint8_t type, uint8_t *buf)
{
	usb_device_t	dev;

	dev = g_root_device;
	dev.addr = addr;
	return usb_get_descriptor_len(&dev, type, 0, buf, 64);
}

static int	usb_set_address_ex(usb_device_t *dev, uint8_t new_addr)
{
	usb_setup_t	setup;
	int			ret;

	setup.bmRequestType = 0x00;
	setup.bRequest = USB_REQ_SET_ADDRESS;
	setup.wValue = new_addr;
	setup.wIndex = 0;
	setup.wLength = 0;
	ret = usb_control_transfer_ex(dev, &setup, NULL);
	if (ret)
		return ret;
	usleep(10000);
	dev->addr = new_addr;
	return 0;
}

int	usb_set_address(uint8_t new_addr)
{
	g_root_device.addr = 0;
	return usb_set_address_ex(&g_root_device, new_addr);
}

static int	usb_set_configuration(usb_device_t *dev, uint8_t config)
{
	usb_setup_t	setup;

	setup.bmRequestType = 0x00;
	setup.bRequest = USB_REQ_SET_CONFIG;
	setup.wValue = config;
	setup.wIndex = 0;
	setup.wLength = 0;
	return usb_control_transfer_ex(dev, &setup, NULL);
}

static int	usb_hid_set_boot_protocol(usb_device_t *dev, uint8_t interface)
{
	usb_setup_t	setup;

	setup.bmRequestType = 0x21;
	setup.bRequest = USB_REQ_SET_PROTOCOL;
	setup.wValue = 0;
	setup.wIndex = interface;
	setup.wLength = 0;
	return usb_control_transfer_ex(dev, &setup, NULL);
}

static int	usb_hub_set_port_feature(usb_device_t *dev, uint8_t port, uint16_t feature)
{
	usb_setup_t	setup;

	setup.bmRequestType = 0x23;
	setup.bRequest = USB_REQ_SET_FEATURE;
	setup.wValue = feature;
	setup.wIndex = port;
	setup.wLength = 0;
	return usb_control_transfer_ex(dev, &setup, NULL);
}

static int	usb_hub_get_port_status(usb_device_t *dev, uint8_t port, uint8_t *buf)
{
	usb_setup_t	setup;

	setup.bmRequestType = 0xa3;
	setup.bRequest = USB_REQ_GET_STATUS;
	setup.wValue = 0;
	setup.wIndex = port;
	setup.wLength = 4;
	return usb_control_transfer_ex(dev, &setup, buf);
}

static uint16_t	usb_u16_from_buf(uint8_t *buf)
{
	return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

static int	usb_hub_reset_port(usb_device_t *hub, uint8_t port, uint8_t *child_low_speed)
{
	uint16_t	status;
	int			ret;
	int			i;

	log("USB: hub detected, powering port %u\n", LOG_INFO,
		(unsigned int)port);
	ret = usb_hub_set_port_feature(hub, port, USB_PORT_POWER);
	if (ret)
		return ret;
	usleep(100000);
	ret = usb_hub_get_port_status(hub, port, g_usb_buf);
	if (ret)
		return ret;
	status = usb_u16_from_buf(g_usb_buf);
	if (!(status & USB_PORT_CONNECTION))
		return -1;
	log("USB: resetting hub port %u\n", LOG_INFO | LOG_INDENT,
		(unsigned int)port);
	ret = usb_hub_set_port_feature(hub, port, USB_PORT_RESET);
	if (ret)
		return ret;
	for (i = 0; i < 20; i++)
	{
		usleep(20000);
		ret = usb_hub_get_port_status(hub, port, g_usb_buf);
		if (ret)
			return ret;
		status = usb_u16_from_buf(g_usb_buf);
		if (status & USB_PORT_ENABLE)
			break ;
	}
	if (!(status & USB_PORT_ENABLE))
	{
		log("USB: hub port %u did not enable after reset\n", LOG_ERROR, (unsigned int)port);
		return -2;
	}
	*child_low_speed = ((status & USB_PORT_LOW_SPEED) != 0);
	log("USB: hub port %u enabled, child speed=%s\n", LOG_INFO | LOG_INDENT, (unsigned int)port, *child_low_speed ? "low" : "full");
	return 0;
}

static int	usb_hub_reset_first_connected_port(usb_device_t *hub, uint8_t *child_low_speed)
{
	uint8_t	port;
	int		ret;

	port = 1;
	while (port <= 4)
	{
		ret = usb_hub_reset_port(hub, port, child_low_speed);
		if (ret == 0)
			return 0;
		port++;
	}
	log("USB: no connected device found on first 4 hub ports\n", LOG_WARNING);
	return -1;
}

static void	usb_parse_config(uint8_t *buf, uint16_t len)
{
	usb_interface_descriptor_t	*interface;
	usb_endpoint_descriptor_t	*endpoint;
	uint8_t						*ptr;
	uint16_t					pos;
	uint8_t						mouse_interface;

	g_mouse.present = 0;
	mouse_interface = 0xff;
	pos = 0;
	while (pos + 2 <= len)
	{
		ptr = buf + pos;
		if (ptr[0] == 0)
			break ;
		if (pos + ptr[0] > len)
			break ;
		if (ptr[1] == USB_DESC_INTERFACE
			&& ptr[0] >= sizeof(usb_interface_descriptor_t))
		{
			interface = (usb_interface_descriptor_t *)ptr;
			if (interface->bInterfaceClass == USB_CLASS_HID
				&& interface->bInterfaceSubClass == USB_HID_BOOT
				&& interface->bInterfaceProtocol == USB_HID_MOUSE)
			{
				mouse_interface = interface->bInterfaceNumber;
				log("USB: HID boot mouse interface=%u endpoints=%u\n", LOG_SUCCESS, (unsigned int)interface->bInterfaceNumber, (unsigned int)interface->bNumEndpoints);
			}
		}
		else if (ptr[1] == USB_DESC_ENDPOINT
			&& ptr[0] >= sizeof(usb_endpoint_descriptor_t)
			&& mouse_interface != 0xff)
		{
			endpoint = (usb_endpoint_descriptor_t *)ptr;
			if ((endpoint->bEndpointAddress & USB_ENDPOINT_IN)
				&& ((endpoint->bmAttributes & USB_ENDPOINT_XFER_MASK) == USB_ENDPOINT_XFER_INTR))
			{
				g_mouse.present = 1;
				g_mouse.interface = mouse_interface;
				g_mouse.endpoint = endpoint->bEndpointAddress & 0x0f;
				g_mouse.interval = endpoint->bInterval;
				g_mouse.mps = endpoint->wMaxPacketSize & 0x7ff;
				if (g_mouse.mps == 0)
					g_mouse.mps = 4;
				log("USB: mouse interrupt IN ep=%u mps=%u interval=%u\n", LOG_INFO | LOG_INDENT, (unsigned int)g_mouse.endpoint, (unsigned int)g_mouse.mps, (unsigned int)g_mouse.interval);
				return;
			}
		}
		pos += ptr[0];
	}
}

int	usb_mouse_poll(void)
{
	int		ret;
	int8_t	dx;
	int8_t	dy;

	if (!g_mouse.present)
		return 0;
	ret = usb_transfer_ex(g_root_device.addr, g_mouse.endpoint, 1, HCCHAR_EPTYPE_INTR, g_mouse.mps, HCTSIZ_PID_DATA0, g_mouse_report, usb_min_u16(g_mouse.mps, sizeof(g_mouse_report)));
	if (ret == -3)
		return 0;
	if (ret)
	{
		log("USB mouse read failed (%i)\n", LOG_WARNING, ret);
		return ret;
	}
	dx = (int8_t)g_mouse_report[1];
	dy = (int8_t)g_mouse_report[2];
	if (dx == 0 && dy == 0 && g_mouse.buttons == g_mouse_report[0])
		return 0;

	// replace last pixel
	if (g_mouse.last_pixel_saved)
	{
		for (int i=0; i<CURSOR_SIZE * CURSOR_SIZE; i++)
			put_pixel(g_mouse.x + (i % CURSOR_SIZE), g_mouse.y + (i / CURSOR_SIZE), g_mouse.last_pixel_color[i]);
	}
	else
		g_mouse.last_pixel_saved = 1;
	g_mouse.x += dx;
	g_mouse.y += dy;

	// clamp cursor
	if (g_mouse.x < 0)
		g_mouse.x = 0;
	else if (g_mouse.x > SCREEN_WIDTH)
		g_mouse.x = SCREEN_WIDTH;
	if (g_mouse.y < 0)
		g_mouse.y = 0;
	else if (g_mouse.y > SCREEN_HEIGHT)
		g_mouse.y = SCREEN_HEIGHT;
	
	g_mouse.buttons = g_mouse_report[0];
	handle_click(g_mouse.x, g_mouse.y, (unsigned int)g_mouse.buttons, get_current_window());
	
	//draw_topbar(get_current_window()); // TODO:
	// save last pixels
	for (int i=0; i<CURSOR_SIZE; i++)
	{
		for (int j=0; j<CURSOR_SIZE; j++)
		{
			g_mouse.last_pixel_color[i * CURSOR_SIZE + j] = get_pixel(g_mouse.x + j, g_mouse.y + i);
			put_pixel(g_mouse.x + j, g_mouse.y + i, DISPLAY_COLORS[RED]);
		}
	}
	return 1;
}

static int	usb_finish_device_configuration(usb_device_t *dev, uint8_t device_class)
{
	usb_config_descriptor_t	*config;
	uint16_t				total_len;
	int						ret;

	ret = usb_get_descriptor_len(dev, USB_DESC_CONFIG, 0, g_usb_buf, sizeof(usb_config_descriptor_t));
	if (ret)
	{
		log("USB: failed to read config descriptor header (%i)\n", LOG_ERROR, ret);
		return ret;
	}
	config = (usb_config_descriptor_t *)g_usb_buf;
	total_len = usb_min_u16(config->wTotalLength, sizeof(g_usb_buf));
	ret = usb_get_descriptor_len(dev, USB_DESC_CONFIG, 0, g_usb_buf, total_len);
	if (ret)
	{
		log("USB: failed to read config descriptor (%i)\n", LOG_ERROR, ret);
		return ret;
	}
	config = (usb_config_descriptor_t *)g_usb_buf;
	log("USB: config=%u interfaces=%u total_len=%u\n",
		LOG_INFO | LOG_INDENT, (unsigned int)config->bConfigurationValue,
		(unsigned int)config->bNumInterfaces,
		(unsigned int)config->wTotalLength);
	usb_parse_config(g_usb_buf, total_len);
	ret = usb_set_configuration(dev, config->bConfigurationValue);
	if (ret)
	{
		log("USB: SET_CONFIGURATION failed (%i)\n", LOG_ERROR, ret);
		return ret;
	}
	if (device_class == USB_CLASS_HUB)
		return 0;
	if (g_mouse.present)
	{
		ret = usb_hid_set_boot_protocol(dev, g_mouse.interface);
		if (ret)
			log("USB: HID SET_PROTOCOL boot failed (%i)\n", LOG_WARNING, ret);
		else
			log("USB: HID mouse ready for polling\n", LOG_SUCCESS);
	}
	else
		log("USB: connected device is not a boot mouse\n", LOG_INFO);
	return 0;
}

static int	usb_enumerate_address_zero(usb_device_t *dev, uint8_t new_addr, uint8_t child_low_speed, uint8_t *device_class)
{
	usb_device_descriptor_t	*device;
	int						ret;

	dev->addr = 0;
	dev->ep0_mps = 8;
	dev->speed_low = child_low_speed;
	ret = usb_get_descriptor_len(dev, USB_DESC_DEVICE, 0, g_usb_buf, 8);
	if (ret)
	{
		log("USB: failed to read initial device descriptor (%i)\n", LOG_ERROR, ret);
		return ret;
	}
	device = (usb_device_descriptor_t *)g_usb_buf;
	dev->ep0_mps = device->bMaxPacketSize0;
	if (dev->ep0_mps == 0)
		dev->ep0_mps = 8;
	ret = usb_set_address_ex(dev, new_addr);
	if (ret)
	{
		log("USB: SET_ADDRESS failed (%i)\n", LOG_ERROR, ret);
		return ret;
	}
	ret = usb_get_descriptor_len(dev, USB_DESC_DEVICE, 0, g_usb_buf, sizeof(usb_device_descriptor_t));
	if (ret)
	{
		log("USB: failed to read full device descriptor (%i)\n", LOG_ERROR, ret);
		return ret;
	}
	device = (usb_device_descriptor_t *)g_usb_buf;
	*device_class = device->bDeviceClass;
	log("USB: device addr=%u vendor=0x%x product=0x%x class=0x%x configs=%u\n",
		LOG_SUCCESS, (unsigned int)dev->addr,
		(unsigned int)device->idVendor, (unsigned int)device->idProduct,
		(unsigned int)device->bDeviceClass,
		(unsigned int)device->bNumConfigurations);
	return usb_finish_device_configuration(dev, *device_class);
}

int	usb_enumerate(void)
{
	usb_device_t			hub;
	uint8_t					device_class;
	uint8_t					child_low_speed;
	int						ret;

	log("USB: initializing host controller\n", LOG_INFO);
	ret = usb_port_reset();
	if (ret)
	{
		log("USB: no enabled device on root port (%i)\n", LOG_WARNING, ret);
		return ret;
	}
	log("USB: root port reset complete, speed=%s\n", LOG_INFO | LOG_INDENT, g_root_device.speed_low ? "low" : "full");
	ret = usb_enumerate_address_zero(&g_root_device, USB_ADDR_FIRST, g_root_device.speed_low, &device_class);
	if (ret)
		return ret;
	if (device_class == USB_CLASS_HUB)
	{
		hub = g_root_device;
		ret = usb_hub_reset_first_connected_port(&hub, &child_low_speed);
		if (ret)
			return ret;
		ret = usb_enumerate_address_zero(&g_root_device, USB_ADDR_CHILD,
				child_low_speed, &device_class);
		if (ret)
			return ret;
	}
	log("USB: enumeration complete\n", LOG_SUCCESS);
	return 0;
}
