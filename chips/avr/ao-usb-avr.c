/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "snek.h"
#include "ao.h"
#include "ao-usb.h"

struct ao_usb_setup {
	uint8_t		dir_type_recip;
	uint8_t		request;
	uint16_t	value;
	uint16_t	index;
	uint16_t	length;
} ao_usb_setup;

static struct ao_fifo ao_usb_rx_fifo;

static CONST uint8_t * ao_usb_ep0_in_data;
static uint8_t 	ao_usb_ep0_in_len;
static bool	ao_usb_ep0_in_pending;
static uint8_t	ao_usb_address;
static uint8_t 	ao_usb_ep0_out_len;
static uint8_t *ao_usb_ep0_out_data;

uint8_t	ao_stdin_ready;
static bool	ao_usb_in_flushed;
uint8_t		ao_usb_running;

#define EP_SIZE(s)	((s) == 64 ? 0x30 :	\
			((s) == 32 ? 0x20 :	\
			((s) == 16 ? 0x10 :	\
			             0x00)))

static void
_ao_usb_set_ep0(void)
{
	/* Set the CONTROL max packet size, single buffered */
	UENUM = 0;
	UECONX = (1 << EPEN);					/* Enable */

	UECFG0X = ((0 << EPTYPE0) |				/* Control */
		   (0 << EPDIR));				/* Out (ish) */

	UECFG1X = (EP_SIZE(AO_USB_CONTROL_SIZE) |		/* Size */
		   (0 << EPBK0) |				/* Single bank */
		   (1 << ALLOC));

	UEIENX = ((1 << RXSTPE) |				/* Enable SETUP interrupt */
		  (1 << RXOUTE) |				/* Enable OUT interrupt */
		  (0 << TXINE));				/* Disable IN interrupt */

	ao_usb_address = 0;
	ao_usb_ep0_in_pending = false;
}

static void
_ao_usb_set_configuration(void)
{
	/* Interrupt EP */
	UENUM = AO_USB_INT_EP;
	UECONX = (1 << EPEN);					/* Enable */

	UECFG0X = ((3 << EPTYPE0) |				/* Control */
		   (1 << EPDIR));				/* In */

	UECFG1X = (EP_SIZE(AO_USB_INT_SIZE) |			/* Size */
		   (0 << EPBK0) |				/* Single bank */
		   (1 << ALLOC));				/* Allocate */

	/* Set the IN max packet size, double buffered */
	UENUM = AO_USB_IN_EP;
	UECONX = (1 << EPEN);					/* Enable */

	UECFG0X = ((2 << EPTYPE0) |				/* Bulk */
		   (1 << EPDIR));				/* In */

	UECFG1X = (EP_SIZE(AO_USB_IN_SIZE) |			/* Size */
		   (1 << EPBK0) |				/* Double bank */
		   (1 << ALLOC));				/* Allocate */

	/* Set the OUT max packet size, double buffered */
	UENUM = AO_USB_OUT_EP;
	UECONX = (1 << EPEN);					/* Enable */

	UECFG0X = ((2 << EPTYPE0) |				/* Bulk */
		   (0 << EPDIR));				/* Out */

	UECFG1X = (EP_SIZE(AO_USB_OUT_SIZE) |			/* Size */
		   (1 << EPBK0) |				/* Double bank */
		   (1 << ALLOC));				/* Allocate */

	UEIENX = (1 << RXOUTE);				/* Enable OUT interrupt */

	ao_usb_running = true;
}

ISR(USB_GEN_vect)
{
	uint8_t udint = UDINT;
	UDINT = 0;
	if (udint & (1 << EORSTI))
		_ao_usb_set_ep0();
}


static CONST struct ao_usb_line_coding ao_usb_line_coding = {115200, 0, 0, 8};

/* Walk through the list of descriptors and find a match
 */
static void
ao_usb_get_descriptor(uint16_t value)
{
	CONST uint8_t	*descriptor;
	uint8_t		type = value >> 8;
	uint8_t		index = value;

	descriptor = ao_usb_descriptors;
	while (descriptor[0] != 0) {
		if (descriptor[1] == type && index-- == 0) {
			if (type == AO_USB_DESC_CONFIGURATION)
				ao_usb_ep0_in_len = descriptor[2];
			else
				ao_usb_ep0_in_len = descriptor[0];
			ao_usb_ep0_in_data = descriptor;
			break;
		}
		descriptor += descriptor[0];
	}
}

/* Send an IN data packet */
static void
_ao_usb_ep0_flush(void)
{
	UENUM = 0;
	if ((UEINTX & (1 << TXINI)) == 0)
		return;

	uint8_t this_len = ao_usb_ep0_in_len;
	if (this_len > AO_USB_CONTROL_SIZE)
		this_len = AO_USB_CONTROL_SIZE;

	ao_usb_ep0_in_len -= this_len;

	if (ao_usb_ep0_in_len == 0 && this_len != AO_USB_CONTROL_SIZE)
		ao_usb_ep0_in_pending = false;

	while (this_len--) {
		uint8_t	c = *ao_usb_ep0_in_data++;
		UEDATX = c;
	}

	/* Clear the TXINI bit to send the packet */
	UEINTX &= ~(1 << TXINI);
}

/* Read data from the ep0 OUT fifo */
static void
_ao_usb_ep0_fill(uint8_t len)
{
	if (len > ao_usb_ep0_out_len)
		len = ao_usb_ep0_out_len;
	ao_usb_ep0_out_len -= len;

	/* Pull all of the data out of the packet */
	UENUM = 0;
	while (len--) {
		uint8_t	c = UEDATX;
		*ao_usb_ep0_out_data++ = c;
	}

	/* ACK the packet */
	UEINTX &= ~((1 << RXSTPI) | (1 << RXOUTI));
}

static void
_ao_usb_ep0_setup(void)
{
	/* Pull the setup packet out of the fifo */
	ao_usb_ep0_out_data = (uint8_t *) &ao_usb_setup;
	ao_usb_ep0_out_len = 8;
	_ao_usb_ep0_fill(8);

	ao_usb_ep0_in_data = 0;
	ao_usb_ep0_in_len = 0;
	switch(ao_usb_setup.request) {
	case AO_USB_REQ_GET_STATUS:
		ao_usb_ep0_in_len = 2;
		break;
	case AO_USB_REQ_SET_ADDRESS:
		ao_usb_address = ao_usb_setup.value;
		break;
	case AO_USB_REQ_GET_DESCRIPTOR:
		ao_usb_get_descriptor(ao_usb_setup.value);
		break;
	case AO_USB_REQ_GET_CONFIGURATION:
		ao_usb_ep0_in_len = 1;
		break;
	case AO_USB_REQ_SET_CONFIGURATION:
		_ao_usb_set_configuration();
		break;
	case AO_USB_REQ_GET_INTERFACE:
		ao_usb_ep0_in_len = 1;
		break;
	case AO_USB_REQ_SET_INTERFACE:
		break;
	case AO_USB_SET_LINE_CODING:
		ao_usb_ep0_out_len = 7;
		ao_usb_ep0_out_data = (uint8_t *) &ao_usb_setup;
		break;
	case AO_USB_GET_LINE_CODING:
		ao_usb_ep0_in_len = 7;
		ao_usb_ep0_in_data = (CONST uint8_t *) &ao_usb_line_coding;
		break;
	case AO_USB_SET_CONTROL_LINE_STATE:
		break;
	}

	/* Queue up an IN packet if the SETUP was IN or the length was 0 (and we need to ack) */
	if ((ao_usb_setup.dir_type_recip & AO_USB_DIR_IN) || ao_usb_setup.length == 0) {
		if (ao_usb_setup.length < ao_usb_ep0_in_len)
			ao_usb_ep0_in_len = ao_usb_setup.length;
		ao_usb_ep0_in_pending = true;
	}
}

/* Wait for a free IN buffer */
static void
_ao_usb_in_wait(void)
{
	for (;;) {
		/* Check if the current buffer is writable */
		UENUM = AO_USB_IN_EP;
		if (UEINTX & (1 << RWAL))
			break;

		/* Wait for an IN buffer to be ready */
		for (;;) {
			UENUM = AO_USB_IN_EP;
			if ((UEINTX & (1 << TXINI)))
				break;
			UEIENX |= (1 << TXINE);
			ao_sleep(&ao_usb_in_flushed);
		}
		/* Ack the interrupt */
		UEINTX &= ~(1 << TXINI);
	}
}

/* Queue the current IN buffer for transmission */
static inline void
_ao_usb_in_send(void)
{
	UEINTX &= ~(1 << FIFOCON);
}

int
ao_usb_flush(FILE *file)
{
	(void) file;
	if (!ao_usb_running)
		return 0;

	ao_arch_block_interrupts();
	/* Anytime we've sent a character since
	 * the last time we flushed, we'll need
	 * to send a packet -- the only other time
	 * we would send a packet is when that
	 * packet was full, in which case we now
	 * want to send an empty packet
	 */
	if (!ao_usb_in_flushed) {
		_ao_usb_in_wait();
		if (!ao_usb_in_flushed) {
			ao_usb_in_flushed = true;
			_ao_usb_in_send();
		}
	}
	ao_arch_release_interrupts();
	return 0;
}

int
ao_usb_putc(char c, FILE *file)
{
	(void) file;

	if (!ao_usb_running)
		return (unsigned char) c;

	if (c == '\n')
		ao_usb_putc('\r', file);

	ao_arch_block_interrupts();
	_ao_usb_in_wait();

	/* Queue a byte */
	UENUM = AO_USB_IN_EP;
	UEDATX = c;

	/* Send the packet when full */
	if ((UEINTX & (1 << RWAL)) == 0)
		_ao_usb_in_send();
	ao_usb_in_flushed = false;
	ao_arch_release_interrupts();
	if (c == '\n')
		ao_usb_flush(file);
	return (unsigned char) c;
}

static void
_ao_usb_fifo_check(void)
{
	while (!ao_fifo_full(&ao_usb_rx_fifo)) {
		UENUM = AO_USB_OUT_EP;
		for (;;) {
			uint8_t intx = UEINTX;
			if (intx & (1 << RWAL))
				break;

			if (intx & (1 << FIFOCON)) {
				/* Ack the last packet */
				UEINTX &= ~(1 << FIFOCON);
			}

			/* Check to see if a packet has arrived */
			if ((intx & (1 << RXOUTI)) == 0) {
				UEIENX |= (1 << RXOUTE);
				return;
			}

			/* Ack the interrupt */
			UEINTX = ~(1 << RXOUTI);
		}

		/* Pull a character out of the fifo */
		uint8_t c = UEDATX;
		if (c == ('c' & 0x1f))
			snek_abort = true;
		ao_fifo_insert(&ao_usb_rx_fifo, c);
	}
}

static int
_ao_usb_pollchar(void)
{
	if (ao_fifo_empty(&ao_usb_rx_fifo))
		return AO_READ_AGAIN;

	uint8_t c = ao_fifo_remove(&ao_usb_rx_fifo);
	_ao_usb_fifo_check();
	return c;
}

int
ao_usb_getc(void)
{
	int	c;

	ao_arch_block_interrupts();
	while ((c = _ao_usb_pollchar()) == AO_READ_AGAIN)
		ao_sleep(&ao_stdin_ready);
	ao_arch_release_interrupts();
	return c;
}

static void
_ao_usb_ep0_handle(void)
{
	UENUM = 0;
	uint8_t intx = UEINTX;
	if (intx & (1 << RXSTPI)) {
		_ao_usb_ep0_setup();
	}
	if (intx & (1 << RXOUTI)) {
		_ao_usb_ep0_fill(UEBCLX);
		ao_usb_ep0_in_pending = true;	/* we need to ack the packet */
	}
	if ((UEIENX & (1 << TXINE)) && (intx & (1 << TXINI))) {
		if (ao_usb_address) {
			UDADDR = (1 << ADDEN) | ao_usb_address;
			ao_usb_address = 0;
		}
	}
	if (ao_usb_ep0_in_pending) {
		_ao_usb_ep0_flush();
		UEIENX |= (1 << TXINE);
	} else {
		UEIENX &= ~(1 << TXINE);
	}
}

/* Endpoint interrupt */
ISR(USB_COM_vect)
{
	uint8_t	i = UEINT;
	UEINT = 0;
	if (i & (1 << 0))
		_ao_usb_ep0_handle();

	if (i & (1 << AO_USB_IN_EP)) {
		UENUM = AO_USB_IN_EP;
		UEIENX = 0;
		ao_wakeup(&ao_usb_in_flushed);
	}

	if (i & (1 << AO_USB_OUT_EP)) {
		UENUM = AO_USB_OUT_EP;
		UEIENX = 0;
		_ao_usb_fifo_check();
		if (!ao_fifo_empty(&ao_usb_rx_fifo))
			ao_wakeup(&ao_stdin_ready);
	}
}

#if F_CPU == 16000000UL
#define AO_USB_PLL_INPUT_PRESCALER	(1 << PINDIV)	/* Divide 16MHz clock by 2 */
#endif
#if F_CPU == 8000000UL
#define AO_USB_PLL_INPUT_PRESCALER	0		/* Don't divide clock */
#endif

#define AO_USB_CON ((1 << USBE) |	/* USB enable */ \
		    (0 << RSTCPU) |	/* do not reset CPU */	\
		    (0 << LSM) |	/* Full speed mode */	\
		    (0 << RMWKUP))	/* no remote wake-up */ \

void
ao_usb_init(void)
{
	/* Configure pad regulator */
	UHWCON = (1 << UVREGE);

	/* Enable USB device, but freeze the clocks until initialized */
	USBCON = AO_USB_CON | (1 <<FRZCLK);

	UDCON = (1 << DETACH);

	/* Enable PLL with appropriate divider */
	PLLCSR = AO_USB_PLL_INPUT_PRESCALER | (1 << PLLE);

	/* Wait for PLL to lock */
	loop_until_bit_is_set(PLLCSR, (1 << PLOCK));

	/* Enable USB, enable the VBUS pad */
	USBCON = AO_USB_CON | (1 << OTGPADE);

	/* Enable global interrupts */
	UDIEN = (1 << EORSTE);		/* End of reset interrupt */

	int i;
	for (i = 0; i < 10000; i++)
		ao_arch_nop();

	UDCON = (0 << DETACH);	/* Clear the DETACH bit to plug into the bus */
}
