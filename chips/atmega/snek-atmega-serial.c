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
 */

#include "snek.h"

/* UART baud rate */
#ifndef UART_BAUD
#define UART_BAUD  		115200
#endif
#define UART_BAUD_U2X(div)	((F_CPU / (div) / UART_BAUD - 1) / 2)
#define UART_U2X		(UART_BAUD_U2X(4) <= 4095)
#define UART_BAUD_SCALE		(UART_U2X ? UART_BAUD_U2X(4) : UART_BAUD_U2X(8))

#ifndef UART_RINGSIZE
#define UART_RINGSIZE	16
#endif

typedef volatile struct uart_ring {
	uint8_t	read;
	uint8_t	count;
	uint8_t	buf[UART_RINGSIZE];
} uart_ring_t;

static uart_ring_t	rx_ring, tx_ring;
static volatile uint8_t rx_flow;
static volatile uint8_t	tx_flow;

/* Start at EMPTY state so we send a ^Q at startup time */
#define FLOW_EMPTY	0
#define FLOW_RUNNING	1
#define FLOW_FULL	2
#define FLOW_STOPPED	3

#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__)
#define UDRE_vect USART0_UDRE_vect
#define RX_vect USART0_RX_vect
#elif defined(__AVR_ATmega4809__)
# define UDRE_vect 	USART3_DRE_vect
# define RX_vect 	USART3_RXC_vect
# define DISABLE_DRE()	(USART3.CTRLA &= ~USART_DREIE_bm)
# define ENABLE_DRE()	(USART3.CTRLA |= USART_DREIE_bm)
# define TX_EMPTY()	(USART3.STATUS & USART_DREIF_bm)
# define RX_DATA	USART3.RXDATAL
# define TX_DATA	USART3.TXDATAL
#else
#define UDRE_vect USART_UDRE_vect
#define RX_vect USART_RX_vect
#endif

#ifndef DISABLE_DRE
#define RX_DATA	UDR0
#define TX_DATA UDR0
#define DISABLE_DRE()	(UCSR0B &= ~(1 << UDRIE0))
#define ENABLE_DRE()	(UCSR0B |= (1 << UDRIE0))
#define TX_EMPTY()	(UCSR0A & (1 << UDRE0))
#endif

static bool ring_full(uart_ring_t *ring)
{
	return ring->count == UART_RINGSIZE;
}

static bool ring_empty(uart_ring_t *ring)
{
	return ring->count == 0;
}

static bool ring_mostly_full(uart_ring_t *ring)
{
	return ring->count >= (UART_RINGSIZE / 2);
}

static int
ring_get(uart_ring_t *ring)
{
	if (ring_empty(ring))
		return -1;
	uint8_t c = ring->buf[ring->read];
	ring->read = (ring->read + 1) & (UART_RINGSIZE - 1);
	ring->count--;
	return c;
}

static bool
ring_put(uart_ring_t *ring, uint8_t c)
{
	if (ring_full(ring))
		return false;
	uint8_t write = (ring->read + ring->count) & (UART_RINGSIZE - 1);
	ring->buf[write] = c;
	ring->count++;
	return true;
}

static void
next_flow(void)
{
	rx_flow = (rx_flow + 1) & 3;
}

static void
_snek_uart_tx_start(void)
{
	if (TX_EMPTY() && !tx_flow) {
		uint8_t c;

		if ((rx_flow & 1) == 0) {
			next_flow();
			if (rx_flow == FLOW_RUNNING)
				c = 'q' & 0x1f;
			else
				c = 's' & 0x1f;
		} else {
			int ic;
			ic = ring_get(&tx_ring);
			if (ic == -1)
				return;
			c = ic;
		}
		ENABLE_DRE();
		TX_DATA = c;
	}
}

static void
_snek_uart_flow_do(void)
{
	next_flow();
	_snek_uart_tx_start();
}

static void
_snek_uart_xon(void)
{
	if (rx_flow == FLOW_STOPPED && ring_empty(&rx_ring))
		_snek_uart_flow_do();
}

static void
_snek_uart_xoff(void)
{
	if (rx_flow == FLOW_RUNNING && ring_mostly_full(&rx_ring))
		_snek_uart_flow_do();
}

ISR(UDRE_vect)
{
	DISABLE_DRE();
	_snek_uart_xon();
	_snek_uart_tx_start();
}

ISR(RX_vect)
{
	uint8_t	c = RX_DATA;

	switch (c) {
	case 'c' & 0x1f:
		snek_abort = true;
		break;
	case 's' & 0x1f:
		tx_flow = true;
		return;
	case 'q' & 0x1f:
		tx_flow = false;
		_snek_uart_tx_start();
		return;
	}

	ring_put(&rx_ring, c);

	_snek_uart_xoff();
}

char
snek_uart_getch(void)
{
	int c;
	for (;;) {
		cli();
		c = ring_get(&rx_ring);
		if (c != -1)
			break;
		sei();
	}
	_snek_uart_xon();
	sei();
	return c;
}

void
snek_uart_putch(char c)
{
	switch (c) {
	case '\n':
		snek_uart_putch('\r');
		break;
	}
	for (;;) {
		cli();
		if (ring_put(&tx_ring, c)) {
			_snek_uart_tx_start();
			sei();
			return;
		}
		sei();
	}
}

int
snek_uart_putchar(char c, FILE *stream)
{
	(void) stream;
	snek_uart_putch(c);
	return 0;
}

void
_snek_uart_puts(const char *PROGMEM string)
{
	char c;
	while ((c = pgm_read_byte(string++)))
		snek_uart_putch(c);
}

void
snek_uart_init(void)
{
#if defined(__AVR_ATmega4809__)
	PORTMUX.USARTROUTEA |= PORTMUX_USART30_bm;

	int32_t baud_setting;
	baud_setting = (((8 * F_CPU) / UART_BAUD) + 1) / 2;
	int8_t sigrow_val = SIGROW.OSC16ERR5V;
	baud_setting += (baud_setting * sigrow_val) / 1024;

	USART3.BAUD = baud_setting;
	USART3.CTRLC = (USART_CHSIZE0_bm | USART_CHSIZE1_bm);
	VPORTB_DIR |= (1 << 4);
	VPORTB_OUT |= (1 << 4);
	USART3.CTRLB = (USART_RXEN_bm |
			USART_TXEN_bm);
	USART3.CTRLA = USART_RXCIE_bm;
#else
	UBRR0H = (uint8_t) (UART_BAUD_SCALE >> 8);
	UBRR0L = (uint8_t) (UART_BAUD_SCALE);
	UCSR0A = ((1 << TXC0) |
		  (UART_U2X << U2X0) |
		  (0 << MPCM0));
	UCSR0C = ((0 << UMSEL01) |
		  (0 << UMSEL00) |
		  (0 << UPM01) |
		  (0 << UPM00) |
		  (0 << USBS0) |
		  (1 << UCSZ00) |
		  (1 << UCSZ01) |
		  (0 << UCPOL0));
	UCSR0B = ((1 << RXCIE0) |
		  (0 << TXCIE0) |
		  (0 << UDRIE0) |
		  (1 << RXEN0) |
		  (1 << TXEN0) |
		  (0 << UCSZ02) |
		  (0 << TXB80));
#endif
	snek_uart_puts("Welcome to Snek " SNEK_VERSION "\n");
}
