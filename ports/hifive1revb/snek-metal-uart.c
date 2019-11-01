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

#include <stdio.h>
#include <snek.h>
#include <metal/machine.h>
#include <metal/uart.h>
#include <metal/interrupt.h>
#include <metal/init.h>

#define disable_interrupt() 	__metal_interrupt_global_disable()
#define enable_interrupt() 	__metal_interrupt_global_enable()
#define wait_interrupt() 	asm("wfi" ::: "memory")

#define RINGSIZE	64

#define UART __METAL_DT_STDOUT_UART_HANDLE

typedef volatile struct uart_ring {
	uint8_t	read;
	uint8_t	count;
	uint8_t	buf[RINGSIZE];
} uart_ring_t;

static uart_ring_t	rx_ring, tx_ring;
static volatile uint8_t rx_flow;
static volatile uint8_t	tx_flow;

/* Start at EMPTY state so we send a ^Q at startup time */
#define FLOW_EMPTY	0
#define FLOW_RUNNING	1
#define FLOW_FULL	2
#define FLOW_STOPPED	3

static bool ring_full(uart_ring_t *ring)
{
	return ring->count == RINGSIZE;
}

static bool ring_empty(uart_ring_t *ring)
{
	return ring->count == 0;
}

static bool ring_mostly_full(uart_ring_t *ring)
{
	return ring->count >= (RINGSIZE / 2);
}

static int
ring_get(uart_ring_t *ring)
{
	if (ring_empty(ring))
		return -1;
	uint8_t c = ring->buf[ring->read];
	ring->read = (ring->read + 1) & (RINGSIZE - 1);
	ring->count--;
	return c;
}

static bool
ring_put(uart_ring_t *ring, uint8_t c)
{
	if (ring_full(ring))
		return false;
	uint8_t write = (ring->read + ring->count) & (RINGSIZE - 1);
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
	if (metal_uart_txready(UART) == 0) {
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
		metal_uart_putc(UART, c);
		metal_uart_transmit_interrupt_enable(UART);
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

int
snek_uart_getc(FILE *f)
{
	(void) f;
	int c;
	for (;;) {
		disable_interrupt();
		c = ring_get(&rx_ring);
		if (c != -1)
			break;
		enable_interrupt();
		wait_interrupt();
	}
	_snek_uart_xon();
	enable_interrupt();
	return c;
}

int
snek_uart_putc(char c, FILE *f)
{
	(void) f;
	if (c == '\n')
		snek_uart_putc('\r', f);
	for (;;) {
		disable_interrupt();
		if (ring_put(&tx_ring, c)) {
			_snek_uart_tx_start();
			enable_interrupt();
			return (uint8_t) c;
		}
		enable_interrupt();
	}
}

static void
snek_uart_isr(int id, void *param)
{
	int ch;

	if (metal_uart_getc(UART, &ch) == 0 && ch != -1) {
		uint8_t c = ch;
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
	if (metal_uart_txready(UART) == 0) {
		metal_uart_transmit_interrupt_disable(UART);
		_snek_uart_xon();
		_snek_uart_tx_start();
	}
}

void
snek_uart_setup(void)
{
	metal_uart_set_transmit_watermark(UART, 1);
	metal_uart_set_receive_watermark(UART, 0);

	struct metal_cpu *cpu = metal_cpu_get(metal_cpu_get_current_hartid());

	struct metal_interrupt *cpu_intr = metal_cpu_interrupt_controller(cpu);
	metal_interrupt_init(cpu_intr);

	struct metal_interrupt *uart_intr = metal_uart_interrupt_controller(UART);
	metal_interrupt_init(uart_intr);

	int uart_id = metal_uart_get_interrupt_id(UART);

	metal_interrupt_register_handler(uart_intr, uart_id, snek_uart_isr, NULL);

	metal_uart_receive_interrupt_enable(UART);
	metal_uart_transmit_interrupt_disable(UART);

	metal_interrupt_enable(uart_intr, uart_id);

	metal_interrupt_enable(cpu_intr, 0);
}

#ifndef __METAL_DT_STDOUT_UART_BAUD
#define __METAL_DT_STDOUT_UART_BAUD 115200
#endif

METAL_CONSTRUCTOR(snek_uart_init) {
	metal_uart_init(UART, __METAL_DT_STDOUT_UART_BAUD);
}
