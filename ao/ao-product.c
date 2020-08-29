/*
 * Copyright © 2009 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include "ao.h"
#include "ao-product.h"

/* Defines which mark this particular AltOS product */

#define LE_WORD(x)    ((x)&0xFF),((uint8_t) (((uint16_t) (x))>>8))

#if HAS_USB

/* Maximum power in mA */
#ifndef AO_USB_MAX_POWER
#define AO_USB_MAX_POWER	100
#endif

#ifndef AO_USB_SELF_POWER
#define AO_USB_SELF_POWER	1
#endif

#define AO_USB_DEVICE_CLASS_COMMUNICATION	0x02
#define AO_USB_INTERFACE_CLASS_CONTROL_CDC	0x02
#define AO_USB_INTERFACE_CLASS_DATA_CDC		0x0A

#ifndef AO_USB_DEVICE_CLASS
#define AO_USB_DEVICE_CLASS		AO_USB_DEVICE_CLASS_COMMUNICATION
#endif

#ifndef AO_USB_INTERFACE_CLASS_DATA
#define AO_USB_INTERFACE_CLASS_CONTROL	AO_USB_INTERFACE_CLASS_CONTROL_CDC
#define AO_USB_INTERFACE_CLASS_DATA	AO_USB_INTERFACE_CLASS_DATA_CDC
#endif

#include "ao-usb.h"

#define HEADER_LEN	       	9
#define CONTROL_CLASS_LEN	35
#define DATA_LEN		(9 + 7 * AO_USB_HAS_OUT + 7 * AO_USB_HAS_IN + 7 * AO_USB_HAS_IN2 + 7 * AO_USB_HAS_IN3)

#define TOTAL_LENGTH		(HEADER_LEN + AO_USB_HAS_INT * CONTROL_CLASS_LEN + DATA_LEN)
#define NUM_INTERFACES		(AO_USB_HAS_INT + 1)

/* USB descriptors in one giant block of bytes */
const uint8_t ao_usb_descriptors [] =
{
	/* Device descriptor */
	0x12,
	AO_USB_DESC_DEVICE,
	LE_WORD(0x0110),	/*  bcdUSB */
	AO_USB_DEVICE_CLASS,	/*  bDeviceClass */
	0x00,			/*  bDeviceSubClass */
	0x00,			/*  bDeviceProtocol */
	AO_USB_CONTROL_SIZE,	/*  bMaxPacketSize */
	LE_WORD(AO_idVendor_NUMBER),	/*  idVendor */
	LE_WORD(AO_idProduct_NUMBER),	/*  idProduct */
	LE_WORD(0x0100),	/*  bcdDevice */
	0x01,			/*  iManufacturer */
	0x02,			/*  iProduct */
	0x03,			/*  iSerialNumber */
	0x01,			/*  bNumConfigurations */

	/* Configuration descriptor */
	0x09,
	AO_USB_DESC_CONFIGURATION,
	LE_WORD(TOTAL_LENGTH),	/*  wTotalLength */
	NUM_INTERFACES,		/*  bNumInterfaces */
	0x01,			/*  bConfigurationValue */
	0x00,			/*  iConfiguration */
	0x80 | (AO_USB_SELF_POWER << 6),	/*  bmAttributes */
	AO_USB_MAX_POWER >> 1,	/*  bMaxPower, 2mA units */

#if AO_USB_HAS_INT
	/* Control class interface */
	0x09,
	AO_USB_DESC_INTERFACE,
	0x00,			/*  bInterfaceNumber */
	0x00,			/*  bAlternateSetting */
	0x01,			/*  bNumEndPoints */
	AO_USB_INTERFACE_CLASS_CONTROL,	/*  bInterfaceClass */
	0x02,			/*  bInterfaceSubClass */
	0x01,			/*  bInterfaceProtocol, linux requires value of 1 for the cdc_acm module */
	0x00,			/*  iInterface */

	/* Header functional descriptor */
	0x05,
	AO_USB_CS_INTERFACE,
	0x00,			/*  bDescriptor SubType Header */
	LE_WORD(0x0110),	/*  CDC version 1.1 */

	/* Call management functional descriptor */
	0x05,
	AO_USB_CS_INTERFACE,
	0x01,			/* bDescriptor SubType Call Management */
	0x01,			/* bmCapabilities = device handles call management */
	0x01,			/* bDataInterface call management interface number */

	/* ACM functional descriptor */
	0x04,
	AO_USB_CS_INTERFACE,
	0x02,			/* bDescriptor SubType Abstract Control Management */
	0x02,			/* bmCapabilities = D1 (Set_line_Coding, Set_Control_Line_State, Get_Line_Coding and Serial_State) */

	/* Union functional descriptor */
	0x05,
	AO_USB_CS_INTERFACE,
	0x06,			/* bDescriptor SubType Union Functional descriptor */
	0x00,			/* bLeaderInterface */
	0x01,			/* bFollowerInterface0 */

	/* Notification EP */
	0x07,
	AO_USB_DESC_ENDPOINT,
	AO_USB_INT_EP|0x80,	/* bEndpointAddress */
	0x03,			/* bmAttributes = intr */
	LE_WORD(8),		/* wMaxPacketSize */
	0xff,			/* bInterval */
#endif

	/* Data class interface descriptor */
	0x09,
	AO_USB_DESC_INTERFACE,
	AO_USB_HAS_INT,			/* bInterfaceNumber */
	0x00,				/* bAlternateSetting */
	AO_USB_HAS_OUT + AO_USB_HAS_IN + AO_USB_HAS_IN2 + AO_USB_HAS_IN3,	/* bNumEndPoints */
	AO_USB_INTERFACE_CLASS_DATA,	/* bInterfaceClass = data */
	0x00,				/* bInterfaceSubClass */
	0x00,				/* bInterfaceProtocol */
	0x00,				/* iInterface */

#if AO_USB_HAS_OUT
	/* Data EP OUT */
	0x07,
	AO_USB_DESC_ENDPOINT,
	AO_USB_OUT_EP,		/* bEndpointAddress */
	0x02,			/* bmAttributes = bulk */
	LE_WORD(AO_USB_OUT_SIZE),/* wMaxPacketSize */
	0x00,			/* bInterval */
#endif

#if AO_USB_HAS_IN
	/* Data EP in */
	0x07,
	AO_USB_DESC_ENDPOINT,
	AO_USB_IN_EP|0x80,	/* bEndpointAddress */
	0x02,			/* bmAttributes = bulk */
	LE_WORD(AO_USB_IN_SIZE),/* wMaxPacketSize */
	0x00,			/* bInterval */
#endif

	/* String descriptors */
	0x04,
	AO_USB_DESC_STRING,
	LE_WORD(0x0409),

	/* iManufacturer */
	AO_iManufacturer_LEN,
	AO_USB_DESC_STRING,
	AO_iManufacturer_UCS2,

	/* iProduct */
	AO_iProduct_LEN,
	AO_USB_DESC_STRING,
	AO_iProduct_UCS2,

	/* iSerial */
	AO_iSerial_LEN,
	AO_USB_DESC_STRING,
	AO_iSerial_UCS2,

	/* Terminating zero */
	0
};
#endif
