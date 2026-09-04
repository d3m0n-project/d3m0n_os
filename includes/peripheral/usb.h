#ifndef USB_H
#define USB_H

#include "types.h"


typedef struct {
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint16_t	wValue;
	uint16_t	wIndex;
	uint16_t	wLength;
}	__attribute__((packed)) usb_setup_t;

typedef struct {
	volatile uint32_t	HCCHAR;
	volatile uint32_t	HCSPLT;
	volatile uint32_t	HCINT;
	volatile uint32_t	HCINTMSK;
	volatile uint32_t	HCTSIZ;
	volatile uint32_t	HCDMA;
	volatile uint32_t	reserved[2];
}	usb_hc_t;

typedef struct {
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint16_t	bcdUSB;
	uint8_t		bDeviceClass;
	uint8_t		bDeviceSubClass;
	uint8_t		bDeviceProtocol;
	uint8_t		bMaxPacketSize0;
	uint16_t	idVendor;
	uint16_t	idProduct;
	uint16_t	bcdDevice;
	uint8_t		iManufacturer;
	uint8_t		iProduct;
	uint8_t		iSerialNumber;
	uint8_t		bNumConfigurations;
}	__attribute__((packed)) usb_device_descriptor_t;

typedef struct {
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint16_t	wTotalLength;
	uint8_t		bNumInterfaces;
	uint8_t		bConfigurationValue;
	uint8_t		iConfiguration;
	uint8_t		bmAttributes;
	uint8_t		bMaxPower;
}	__attribute__((packed)) usb_config_descriptor_t;

typedef struct {
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bInterfaceNumber;
	uint8_t		bAlternateSetting;
	uint8_t		bNumEndpoints;
	uint8_t		bInterfaceClass;
	uint8_t		bInterfaceSubClass;
	uint8_t		bInterfaceProtocol;
	uint8_t		iInterface;
}	__attribute__((packed)) usb_interface_descriptor_t;

typedef struct {
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bEndpointAddress;
	uint8_t		bmAttributes;
	uint16_t	wMaxPacketSize;
	uint8_t		bInterval;
}	__attribute__((packed)) usb_endpoint_descriptor_t;


void	usb_init(void);
int		usb_enumerate(void);
int		usb_mouse_poll(void);
int		usb_control_transfer(uint8_t addr, usb_setup_t *setup, uint8_t *buf);
int		usb_get_descriptor(uint8_t addr, uint8_t type, uint8_t *buf);
int		usb_set_address(uint8_t new_addr);

#endif
