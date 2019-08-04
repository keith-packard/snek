/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
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
#include "ao-usb.h"
#include "ao-product.h"

#ifndef AO_POWER_MANAGEMENT
#define AO_POWER_MANAGEMENT	0
#endif

#ifndef AO_USB_DEVICE_ID_SERIAL
#define AO_USB_DEVICE_ID_SERIAL 1
#endif

#if USE_USB_FIFO
static struct ao_fifo	ao_usb_rx_fifo;
#endif

#define AO_USB_OUT_SLEEP_ADDR	(&ao_usb_out_avail)

#define SAMD21_USB_ALIGN	__attribute__ ((aligned(4)))

struct ao_usb_setup {
	uint8_t		dir_type_recip;
	uint8_t		request;
	uint16_t	value;
	uint16_t	index;
	uint16_t	length;
} ao_usb_setup;

static uint8_t 	ao_usb_ep0_state;

struct samd21_usb_desc	samd21_usb_desc[8] SAMD21_USB_ALIGN;

/* Pending EP0 IN data */
static uint8_t		ao_usb_ep0_in_tmp[2];	/* small buffer */
static const uint8_t	*ao_usb_ep0_in_data;	/* Remaining data */
static uint8_t 		ao_usb_ep0_in_len;	/* Remaining amount */

/* Pending EP0 OUT data */
static uint8_t *ao_usb_ep0_out_data;
static uint8_t 	ao_usb_ep0_out_len;

/* Endpoint 0 buffers */
static uint8_t ao_usb_ep0_in_buf[AO_USB_CONTROL_SIZE] SAMD21_USB_ALIGN;
static uint8_t ao_usb_ep0_out_buf[AO_USB_CONTROL_SIZE]  SAMD21_USB_ALIGN;

#if AO_USB_HAS_INT
/* Pointer to interrupt buffer in USB memory */
static uint8_t ao_usb_int_buf[AO_USB_INT_SIZE]  SAMD21_USB_ALIGN;
#endif

/* Buffers in DRAM */
#if AO_USB_HAS_IN
static uint8_t	ao_usb_in_tx_which;
static uint8_t	ao_usb_tx_count;
static uint8_t	ao_usb_in_buf[2][AO_USB_IN_SIZE]  SAMD21_USB_ALIGN;

#endif
#if AO_USB_HAS_OUT
static uint8_t	ao_usb_out_rx_which;
#if !USE_USB_FIFO
static uint8_t	ao_usb_rx_count, ao_usb_rx_pos;
#endif
static uint8_t	ao_usb_out_buf[2][AO_USB_OUT_SIZE]  SAMD21_USB_ALIGN;

#endif

/* Marks when we don't need to send an IN packet.
 * This happens only when the last IN packet is not full,
 * otherwise the host will expect to keep seeing packets.
 * Send a zero-length packet as required
 */
static uint8_t	ao_usb_in_flushed;

/* Marks when we have delivered an IN packet to the hardware
 * and it has not been received yet. ao_sleep on this address
 * to wait for it to be delivered.
 */
static uint8_t	ao_usb_in_pending;

/* Marks when an OUT packet has been received by the hardware
 * but not pulled to the shadow buffer.
 */
static uint8_t	ao_usb_out_avail;
uint8_t		ao_usb_running;
static uint8_t	ao_usb_configuration;

#define AO_USB_EP0_GOT_SETUP	1
#define AO_USB_EP0_GOT_RX_DATA	2
#define AO_USB_EP0_GOT_TX_ACK	4

static uint8_t	ao_usb_ep0_receive;
static uint8_t	ao_usb_address;
static uint8_t	ao_usb_address_pending;

/*
 * Set current device address and mark the
 * interface as active
 */
static void
ao_usb_set_address(uint8_t address)
{
	samd21_usb.dadd = (1 << SAMD21_USB_DADD_ADDEN) | (address << SAMD21_USB_DADD_DADD);
	ao_usb_address_pending = 0;
}

/*
 * Initialize an entpoint
 */

static void
ao_usb_init_bank(struct samd21_usb_desc_bank *bank,
		 uint8_t *buf,
		 uint16_t size)
{
	bank->addr = (uint32_t) buf;

	uint32_t size_bits = 0;
	switch (size) {
	case 8: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_8; break;
	case 16: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_16; break;
	case 32: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_32; break;
	case 64: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_64; break;
	case 128: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_128; break;
	case 256: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_256; break;
	case 512: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_512; break;
	case 1023: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_1023; break;
	}
	bank->pcksize = ((0 << SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT) |
			 (0 << SAMD21_USB_DESC_PCKSIZE_MULTI_PACKET_SIZE) |
			 (size_bits << SAMD21_USB_DESC_PCKSIZE_SIZE) |
			 (0 << SAMD21_USB_DESC_PCKSIZE_AUTO_ZLP));
}

static void
ao_usb_init_ep(uint8_t ep,
	       uint8_t type_out, void *out_buf, uint16_t out_size,
	       uint8_t type_in, void *in_buf, uint16_t in_size)
{
	/* set up descriptors */
	ao_usb_init_bank(&samd21_usb_desc[ep].bank[0],
			 out_buf, out_size);

	ao_usb_init_bank(&samd21_usb_desc[ep].bank[1],
			 in_buf, in_size);

	samd21_usb.ep[ep].epcfg = ((type_out << SAMD21_USB_EP_EPCFG_EP_TYPE_OUT) |
				   (type_in << SAMD21_USB_EP_EPCFG_EP_TYPE_IN));

	/* Clear all status bits */
	samd21_usb.ep[ep].epstatusclr = 0xff;

	/* Select interrupts */
	uint8_t	epinten = 0;
	if (out_buf)
		epinten |= (1 << SAMD21_USB_EP_EPINTFLAG_TRCPT0);
	if (in_buf)
		epinten |= (1 << SAMD21_USB_EP_EPINTFLAG_TRCPT1);
	if (ep == 0)
		epinten |= (1 << SAMD21_USB_EP_EPINTFLAG_RXSTP);
	samd21_usb.ep[ep].epintenset = epinten;
}

static void
ao_usb_set_ep0(void)
{
	ao_usb_init_ep(AO_USB_CONTROL_EP,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_CONTROL,
		       ao_usb_ep0_out_buf, AO_USB_CONTROL_SIZE,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_IN_CONTROL,
		       ao_usb_ep0_in_buf, AO_USB_CONTROL_SIZE);

	ao_usb_running = 0;

	/* Reset our internal state
	 */

	ao_usb_ep0_state = AO_USB_EP0_IDLE;

	ao_usb_ep0_in_data = NULL;
	ao_usb_ep0_in_len = 0;

	ao_usb_ep0_out_data = NULL;
	ao_usb_ep0_out_len = 0;
}

static void
ao_usb_set_configuration(void)
{
#if AO_USB_HAS_INT
	ao_usb_init_ep(AO_USB_INT_EP,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_DISABLED,
		       NULL, 0,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_IN_INTERRUPT,
		       ao_usb_int_buf, AO_USB_INT_SIZE);
#endif

#if AO_USB_HAS_OUT
	ao_usb_init_ep(AO_USB_OUT_EP,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_BULK,
		       &ao_usb_out_buf[0][0], AO_USB_OUT_SIZE,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_IN_DUAL_BANK,
		       &ao_usb_out_buf[1][0], AO_USB_OUT_SIZE);

	/* At first receive, we'll flip this back to 0 */
	ao_usb_out_rx_which = 1;
#endif

#if AO_USB_HAS_IN
	/* Set up the IN end point */
	ao_usb_init_ep(AO_USB_IN_EP,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_DUAL_BANK,
		       &ao_usb_in_buf[0][0], AO_USB_IN_SIZE,
		       SAMD21_USB_EP_EPCFG_EP_TYPE_IN_BULK,
		       &ao_usb_in_buf[1][0], AO_USB_IN_SIZE);

	/* First transmit data goes to buffer 0 */
	ao_usb_in_tx_which = 0;
#endif

	ao_usb_in_flushed = 1;
	ao_usb_in_pending = 0;
	ao_wakeup(&ao_usb_in_pending);

	ao_usb_out_avail = 0;
	ao_usb_configuration = 0;

	ao_wakeup(AO_USB_OUT_SLEEP_ADDR);
}

/* Send an IN data packet */
static void
ao_usb_ep0_flush(void)
{
	uint8_t this_len;

	/* Check to see if the endpoint is still busy */
	if ((samd21_usb.ep[0].epstatus & (1 << (SAMD21_USB_EP_EPSTATUS_BK1RDY))) != 0)
		return;

	this_len = ao_usb_ep0_in_len;
	if (this_len > AO_USB_CONTROL_SIZE)
		this_len = AO_USB_CONTROL_SIZE;

	if (this_len < AO_USB_CONTROL_SIZE)
		ao_usb_ep0_state = AO_USB_EP0_IDLE;

	ao_usb_ep0_in_len -= this_len;

	memcpy(ao_usb_ep0_in_buf, ao_usb_ep0_in_data, this_len);
	ao_usb_ep0_in_data += this_len;

	/* Mark the endpoint as TX valid to send the packet */
	samd21_usb_desc_set_byte_count(AO_USB_CONTROL_EP, 1, this_len);
	samd21_usb_ep_set_ready(AO_USB_CONTROL_EP, 1);
}

/* Read data from the ep0 OUT fifo */
static void
ao_usb_ep0_fill(void)
{
	uint16_t	len = samd21_usb_desc_get_byte_count(AO_USB_CONTROL_EP, 0);

	if (len > ao_usb_ep0_out_len)
		len = ao_usb_ep0_out_len;
	ao_usb_ep0_out_len -= len;

	/* Pull all of the data out of the packet */
	memcpy(ao_usb_ep0_out_data, ao_usb_ep0_out_buf, len);
	ao_usb_ep0_out_data += len;

	/* ACK the packet */
	samd21_usb_ep_clr_ready(AO_USB_CONTROL_EP, 0);
}

static void
ao_usb_ep0_in_reset(void)
{
	ao_usb_ep0_in_data = ao_usb_ep0_in_tmp;
	ao_usb_ep0_in_len = 0;
}

static void
ao_usb_ep0_in_queue_byte(uint8_t a)
{
	if (ao_usb_ep0_in_len < sizeof (ao_usb_ep0_in_tmp))
		ao_usb_ep0_in_tmp[ao_usb_ep0_in_len++] = a;
}

static void
ao_usb_ep0_in_set(const uint8_t *data, uint8_t len)
{
	ao_usb_ep0_in_data = data;
	ao_usb_ep0_in_len = len;
}

static void
ao_usb_ep0_out_set(uint8_t *data, uint8_t len)
{
	ao_usb_ep0_out_data = data;
	ao_usb_ep0_out_len = len;
}

static void
ao_usb_ep0_in_start(uint16_t max)
{
	/* Don't send more than asked for */
	if (ao_usb_ep0_in_len > max)
		ao_usb_ep0_in_len = max;
	ao_usb_ep0_flush();
}

struct ao_usb_line_coding ao_usb_line_coding = {115200, 0, 0, 8};

#if AO_USB_DEVICE_ID_SERIAL
static uint8_t ao_usb_serial[2 + 64];

/* Convert a 32-bit value to 8 hexidecimal UCS2 characters */
static void
hex_to_ucs2(uint32_t in, uint8_t *out)
{
	int	i;

	for (i = 28; i >= 0; i -= 4) {
		uint8_t	bits = (in >> i) & 0xf;
		*out++ = ((bits < 10) ? '0' : ('a' - 10)) + bits;
		*out++ = 0;
	}
}

/* Encode the device ID (128 bits) in hexidecimal to use as a device
 * serial number
 */
static void
ao_usb_serial_init(void)
{
	ao_usb_serial[0] = 66;	/* length */
	ao_usb_serial[1] = AO_USB_DESC_STRING;
	hex_to_ucs2(samd21_serial.word0, ao_usb_serial + 2 + 0);
	hex_to_ucs2(samd21_serial.word1, ao_usb_serial + 2 + 16);
	hex_to_ucs2(samd21_serial.word2, ao_usb_serial + 2 + 32);
	hex_to_ucs2(samd21_serial.word3, ao_usb_serial + 2 + 48);
}
#endif

/* Walk through the list of descriptors and find a match
 */
static void
ao_usb_get_descriptor(uint16_t value, uint16_t length)
{
	const uint8_t		*descriptor;
	uint8_t		type = value >> 8;
	uint8_t		index = value;

	descriptor = ao_usb_descriptors;
	while (descriptor[0] != 0) {
		if (descriptor[1] == type && index-- == 0) {
			uint8_t	len;
			if (type == AO_USB_DESC_CONFIGURATION)
				len = descriptor[2];
			else
				len = descriptor[0];
#if AO_USB_DEVICE_ID_SERIAL
			/* Slightly hacky - the serial number is string 3 */
			if (type == AO_USB_DESC_STRING && (value & 0xff) == 3) {
				descriptor = ao_usb_serial;
				len = sizeof (ao_usb_serial);
			}
#endif
			if (len > length)
				len = length;
			ao_usb_ep0_in_set(descriptor, len);
			break;
		}
		descriptor += descriptor[0];
	}
}

static void
ao_usb_ep0_setup(void)
{
	/* Pull the setup packet out of the fifo */
	ao_usb_ep0_out_set((uint8_t *) &ao_usb_setup, 8);
	ao_usb_ep0_fill();
	if (ao_usb_ep0_out_len != 0)
		return;

	if ((ao_usb_setup.dir_type_recip & AO_USB_DIR_IN) || ao_usb_setup.length == 0)
		ao_usb_ep0_state = AO_USB_EP0_DATA_IN;
	else
		ao_usb_ep0_state = AO_USB_EP0_DATA_OUT;

	ao_usb_ep0_in_reset();

	switch(ao_usb_setup.dir_type_recip & AO_USB_SETUP_TYPE_MASK) {
	case AO_USB_TYPE_STANDARD:
		switch(ao_usb_setup.dir_type_recip & AO_USB_SETUP_RECIP_MASK) {
		case AO_USB_RECIP_DEVICE:
			switch(ao_usb_setup.request) {
			case AO_USB_REQ_GET_STATUS:
				ao_usb_ep0_in_queue_byte(0);
				ao_usb_ep0_in_queue_byte(0);
				break;
			case AO_USB_REQ_SET_ADDRESS:
				ao_usb_address = ao_usb_setup.value;
				ao_usb_address_pending = 1;
				break;
			case AO_USB_REQ_GET_DESCRIPTOR:
				ao_usb_get_descriptor(ao_usb_setup.value, ao_usb_setup.length);
				break;
			case AO_USB_REQ_GET_CONFIGURATION:
				ao_usb_ep0_in_queue_byte(ao_usb_configuration);
				break;
			case AO_USB_REQ_SET_CONFIGURATION:
				ao_usb_configuration = ao_usb_setup.value;
				ao_usb_set_configuration();
				break;
			}
			break;
		case AO_USB_RECIP_INTERFACE:
			switch(ao_usb_setup.request) {
			case AO_USB_REQ_GET_STATUS:
				ao_usb_ep0_in_queue_byte(0);
				ao_usb_ep0_in_queue_byte(0);
				break;
			case AO_USB_REQ_GET_INTERFACE:
				ao_usb_ep0_in_queue_byte(0);
				break;
			case AO_USB_REQ_SET_INTERFACE:
				break;
			}
			break;
		case AO_USB_RECIP_ENDPOINT:
			switch(ao_usb_setup.request) {
			case AO_USB_REQ_GET_STATUS:
				ao_usb_ep0_in_queue_byte(0);
				ao_usb_ep0_in_queue_byte(0);
				break;
			}
			break;
		}
		break;
	case AO_USB_TYPE_CLASS:
		switch (ao_usb_setup.request) {
		case AO_USB_SET_LINE_CODING:
			ao_usb_ep0_out_set((uint8_t *) &ao_usb_line_coding, 7);
			break;
		case AO_USB_GET_LINE_CODING:
			ao_usb_ep0_in_set((const uint8_t *) &ao_usb_line_coding, 7);
			break;
		case AO_USB_SET_CONTROL_LINE_STATE:
			break;
		}
		break;
	}

	/* If we're not waiting to receive data from the host,
	 * queue an IN response
	 */
	if (ao_usb_ep0_state == AO_USB_EP0_DATA_IN)
		ao_usb_ep0_in_start(ao_usb_setup.length);
}

static void
ao_usb_ep0_handle(uint8_t receive)
{
	ao_usb_ep0_receive = 0;
	if (receive & AO_USB_EP0_GOT_SETUP) {
		ao_usb_ep0_setup();
	}
	if (receive & AO_USB_EP0_GOT_RX_DATA) {
		if (ao_usb_ep0_state == AO_USB_EP0_DATA_OUT) {
			ao_usb_ep0_fill();
			if (ao_usb_ep0_out_len == 0) {
				ao_usb_ep0_state = AO_USB_EP0_DATA_IN;
				ao_usb_ep0_in_start(0);
			}
		}
	}
	if (receive & AO_USB_EP0_GOT_TX_ACK) {

#if HAS_FLIGHT && AO_USB_FORCE_IDLE
		ao_flight_force_idle = 1;
#endif
		if (ao_usb_ep0_state == AO_USB_EP0_DATA_IN)
			ao_usb_ep0_flush();
		/* Wait until the IN packet is received from addr 0
		 * before assigning our local address
		 */
		if (ao_usb_address_pending)
			ao_usb_set_address(ao_usb_address);
	}
}

#if AO_POWER_MANAGEMENT
static void
ao_usb_suspend(void)
{
	stm_usb.cntr |= (1 << STM_USB_CNTR_FSUSP);
	ao_power_suspend();
	stm_usb.cntr |= (1 << STM_USB_CNTR_LP_MODE);
	ao_clock_suspend();
}

static void
ao_usb_wakeup(void)
{
	ao_clock_resume();
	stm_usb.cntr &= ~(1 << STM_USB_CNTR_FSUSP);
	ao_power_resume();
}
#endif

#if USE_USB_FIFO
static void
ao_usb_fifo_check(void)
{
	uint8_t next_which = 1 - ao_usb_out_rx_which;
	if (ao_usb_out_avail & (1 << next_which)) {
		uint8_t *buf = ao_usb_out_buf[next_which];
		uint16_t len = samd21_usb_desc_get_byte_count(AO_USB_OUT_EP, next_which);

		if (ao_fifo_has_space(&ao_usb_rx_fifo, len)) {
			uint16_t	i;

#if AO_USB_OUT_HOOK
			ao_usb_out_hook(buf, len);
#endif
			for (i = 0; i < len; i++)
				ao_fifo_insert(&ao_usb_rx_fifo, buf[i]);
			samd21_usb_ep_clr_ready(AO_USB_OUT_EP, next_which);
			ao_usb_out_avail &= ~(1 << next_which);
			ao_usb_out_rx_which = next_which;
			ao_wakeup(AO_USB_OUT_SLEEP_ADDR);
		}
	}
}
#endif

void
samd21_usb_isr(void)
{
	uint16_t	intflag = samd21_usb.intflag;
	uint16_t	epintsmry = samd21_usb.epintsmry;

	samd21_usb.intflag = intflag;

	if (epintsmry & (1 << 0)) {
		uint8_t	epintflag = samd21_usb.ep[0].epintflag;
		samd21_usb.ep[0].epintflag = epintflag;

		if (epintflag & (1 << SAMD21_USB_EP_EPINTFLAG_RXSTP))
			ao_usb_ep0_receive |= AO_USB_EP0_GOT_SETUP;
		else if (epintflag & (1 << SAMD21_USB_EP_EPINTFLAG_TRCPT0))
			ao_usb_ep0_receive |= AO_USB_EP0_GOT_RX_DATA;
		if (epintflag & (1 << SAMD21_USB_EP_EPINTFLAG_TRCPT1))
			ao_usb_ep0_receive |= AO_USB_EP0_GOT_TX_ACK;
		ao_usb_ep0_handle(ao_usb_ep0_receive);
	}
#if AO_USB_HAS_OUT
	if (epintsmry & (1 << AO_USB_OUT_EP)) {
		uint8_t epintflag = samd21_usb.ep[AO_USB_OUT_EP].epintflag;
		samd21_usb.ep[AO_USB_OUT_EP].epintflag = epintflag;
		uint8_t avail = (epintflag >> SAMD21_USB_EP_EPINTFLAG_TRCPT0) & 3;
		if (avail) {
			ao_usb_out_avail |= avail;
			ao_usb_running = 1;
#if USE_USB_FIFO
			ao_usb_fifo_check();
#else
			ao_wakeup(AO_USB_OUT_SLEEP_ADDR);
#endif
		}
	}
#endif
#if AO_USB_HAS_IN
	if (epintsmry & (1 << AO_USB_IN_EP)) {
		uint8_t epintflag = samd21_usb.ep[AO_USB_IN_EP].epintflag;
		samd21_usb.ep[AO_USB_IN_EP].epintflag = epintflag;
		uint8_t done = (epintflag >> SAMD21_USB_EP_EPINTFLAG_TRCPT0) & 3;
		if (done) {
			ao_usb_in_pending &= ~done;
			ao_wakeup(&ao_usb_in_pending);
		}
	}
#endif
#if AO_USB_HAS_INT
	if (epintsmry & (1 << AO_USB_INT_EP)) {
	}
#endif
	if (intflag & (1 << SAMD21_USB_INTFLAG_EORST)) {
		ao_usb_set_ep0();
	}
#if AO_POWER_MANAGEMENT
	if (istr & (1 << STM_USB_ISTR_SUSP))
		ao_usb_suspend();

	if (istr & (1 << STM_USB_ISTR_WKUP))
		ao_usb_wakeup();
#endif
}

#if AO_USB_HAS_IN
/* Queue the current IN buffer for transmission */
static void
_ao_usb_in_send(void)
{
	ao_usb_in_pending |= 1 << ao_usb_in_tx_which;
	if (ao_usb_tx_count != AO_USB_IN_SIZE)
		ao_usb_in_flushed = 1;

	samd21_usb_desc_set_byte_count(AO_USB_IN_EP, ao_usb_in_tx_which, ao_usb_tx_count);
	samd21_usb_ep_set_ready(AO_USB_IN_EP, ao_usb_in_tx_which);

	/* Toggle our usage */
	ao_usb_in_tx_which = 1 - ao_usb_in_tx_which;
	ao_usb_tx_count = 0;
}

/* Wait for a free IN buffer. Interrupts are blocked */
static void
_ao_usb_in_wait(void)
{
	/* Wait for an IN buffer to be ready */
	while ((ao_usb_in_pending & (1 << ao_usb_in_tx_which)) != 0)
		ao_sleep(&ao_usb_in_pending);
}

int
ao_usb_flush(FILE *file)
{
	(void) file;
	if (!ao_usb_running)
		return 0;

	/* Anytime we've sent a character since
	 * the last time we flushed, we'll need
	 * to send a packet -- the only other time
	 * we would send a packet is when that
	 * packet was full, in which case we now
	 * want to send an empty packet
	 */
	ao_arch_block_interrupts();
	if (!ao_usb_in_flushed) {
		_ao_usb_in_wait();
		if (!ao_usb_in_flushed)
			_ao_usb_in_send();
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

	ao_usb_in_flushed = 0;
	ao_usb_in_buf[ao_usb_in_tx_which][ao_usb_tx_count++] = c;

	/* Send the packet when full */
	if (ao_usb_tx_count == AO_USB_IN_SIZE)
		_ao_usb_in_send();

	ao_arch_release_interrupts();
	if (c == '\n')
		ao_usb_flush(file);
	return (unsigned char) c;
}
#endif

#if AO_USB_HAS_OUT
#if !USE_USB_FIFO
static bool
_ao_usb_out_recv(void)
{
	uint8_t next_which = 1 - ao_usb_out_rx_which;

	if ((ao_usb_out_avail & (1 << next_which)) != 0) {
		/* switch current buffer */
		ao_usb_out_rx_which = next_which;
		ao_usb_out_avail &= ~(1 << ao_usb_out_rx_which);

		ao_usb_rx_count = samd21_usb_desc_get_byte_count(AO_USB_OUT_EP, ao_usb_out_rx_which);
		ao_usb_rx_pos = 0;
		return true;
	}
	return false;
}
#endif

static int
_ao_usb_pollchar(void)
{
	uint8_t c;

	if (!ao_usb_running)
		return AO_READ_AGAIN;

#if USE_USB_FIFO
	if (ao_fifo_empty(&ao_usb_rx_fifo))
		return AO_READ_AGAIN;
	c = ao_fifo_remove(&ao_usb_rx_fifo);
	ao_usb_fifo_check();
#else
	for (;;) {
		if (ao_usb_rx_pos != ao_usb_rx_count)
			break;

		/* Check for packet */
		if (!_ao_usb_out_recv())
			return AO_READ_AGAIN;
	}

	/* Pull a character out of the fifo */
	c = ao_usb_out_buf[ao_usb_out_rx_which][ao_usb_rx_pos++];

	if (ao_usb_rx_pos == ao_usb_rx_count)
		samd21_usb_ep_clr_ready(AO_USB_OUT_EP, ao_usb_out_rx_which);
#endif

	return c;
}

int
ao_usb_getc(void)
{
	int	c;

	ao_arch_block_interrupts();
	while ((c = _ao_usb_pollchar()) == AO_READ_AGAIN)
		ao_sleep(AO_USB_OUT_SLEEP_ADDR);
	ao_arch_release_interrupts();
	return c;
}

bool
ao_usb_waiting(void)
{
	bool waiting;

	ao_arch_block_interrupts();
	waiting = !ao_fifo_empty(&ao_usb_rx_fifo);
	ao_arch_release_interrupts();
	return waiting;
}

#endif

void
ao_usb_disable(void)
{
	ao_arch_block_interrupts();
	samd21_nvic_clear_enable(SAMD21_NVIC_ISR_USB_POS);

	/* Disable USB pull-up */
	samd21_usb.ctrlb |= (1 << SAMD21_USB_CTRLB_DETACH);

	/* Switch off the device */
	samd21_usb.ctrla &= ~(1 << SAMD21_USB_CTRLA_ENABLE);

	/* Disable the interface */
	samd21_pm.apbbmask &= ~(1 << SAMD21_PM_APBBMASK_USB);
	ao_arch_release_interrupts();
}

void
ao_usb_enable(void)
{
	int	t;

	/* Enable USB clock */
	samd21_pm.apbbmask |= (1 << SAMD21_PM_APBBMASK_USB);

	/* Set up USB DM/DP pins */
	samd21_port_pmux_set(&samd21_port_a, 24, SAMD21_PORT_PMUX_FUNC_G);
	samd21_port_pmux_set(&samd21_port_a, 25, SAMD21_PORT_PMUX_FUNC_G);

	/* Assign gclk 0 to USB reference */
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_USB);

	/* Reset USB Device */

	samd21_usb.ctrla |= (1 << SAMD21_USB_CTRLA_SWRST);

	while ((samd21_usb.syncbusy & (1 << SAMD21_USB_SYNCBUSY_SWRST)) == 0)
		;

	while ((samd21_usb.syncbusy & (1 << SAMD21_USB_SYNCBUSY_SWRST)) != 0)
		;

	memset(&samd21_usb_desc, 0, sizeof (samd21_usb_desc));

	/* Detach */
	samd21_usb.ctrlb |= (1 << SAMD21_USB_CTRLB_DETACH);

	samd21_usb.descadd = (uint32_t) &samd21_usb_desc;

	/* Load calibration values */
	uint32_t transn = ((samd21_aux1.calibration >> SAMD21_AUX1_CALIBRATION_USB_TRANSN) &
			   SAMD21_AUX1_CALIBRATION_USB_TRANSN_MASK);
	if (transn == 0x1f)
		transn = 5;
	uint32_t transp = ((samd21_aux1.calibration >> SAMD21_AUX1_CALIBRATION_USB_TRANSP) &
			   SAMD21_AUX1_CALIBRATION_USB_TRANSP_MASK);
	if (transp == 0x1f)
		transp = 29;
	uint32_t trim   = ((samd21_aux1.calibration >> SAMD21_AUX1_CALIBRATION_USB_TRIM) &
			   SAMD21_AUX1_CALIBRATION_USB_TRIM_MASK);
	if (trim == 7)
		trim = 3;

	samd21_usb.padcal = ((transn << SAMD21_USB_PADCAL_TRANSN) |
			     (transp << SAMD21_USB_PADCAL_TRANSP) |
			     (trim << SAMD21_USB_PADCAL_TRIM));

	samd21_usb.qosctrl = ((3 << SAMD21_USB_QOSCTRL_CQOS) |
			      (3 << SAMD21_USB_QOSCTRL_DQOS));

	ao_arch_block_interrupts();

	/* set full speed */
	samd21_usb.ctrlb = (SAMD21_USB_CTRLB_SPDCONF_FS << SAMD21_USB_CTRLB_SPDCONF);

	/* Set device mode */
	samd21_usb.ctrla = ((0 << SAMD21_USB_CTRLA_MODE) |
			    (1 << SAMD21_USB_CTRLA_RUNSTDBY) |
			    (1 << SAMD21_USB_CTRLA_ENABLE));

	while ((samd21_usb.syncbusy & (1 << SAMD21_USB_SYNCBUSY_ENABLE)) != 0)
		;

	/* configure interrupts */
	samd21_nvic_set_enable(SAMD21_NVIC_ISR_USB_POS);
	samd21_nvic_set_priority(SAMD21_NVIC_ISR_USB_POS, 2);

	samd21_usb.intenset = ((1 << SAMD21_USB_INTFLAG_EORST));

	ao_arch_release_interrupts();

	for (t = 0; t < 50000; t++)
		ao_arch_nop();

	/* Attach */
	samd21_usb.ctrlb &= ~(1 << SAMD21_USB_CTRLB_DETACH);
}

void
ao_usb_init(void)
{
	ao_usb_enable();

#if AO_USB_DEVICE_ID_SERIAL
	ao_usb_serial_init();
#endif

	ao_usb_ep0_state = AO_USB_EP0_IDLE;
}
