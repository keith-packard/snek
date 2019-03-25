/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#ifndef _AO_SERIAL_H_
#define _AO_SERIAL_H_

#define AO_SERIAL_SPEED_4800	0
#define AO_SERIAL_SPEED_9600	1
#define AO_SERIAL_SPEED_19200	2
#define AO_SERIAL_SPEED_57600	3
#define AO_SERIAL_SPEED_115200	4

#if HAS_SERIAL_0
extern volatile struct ao_fifo	ao_serial0_rx_fifo;
extern volatile struct ao_fifo	ao_serial0_tx_fifo;

char
ao_serial0_getchar(void);

int
_ao_serial0_pollchar(void);

uint8_t
_ao_serial0_sleep_for(uint16_t timeout);

void
ao_serial0_putchar(char c);

void
ao_serial0_drain(void);

void
ao_serial0_set_speed(uint8_t speed);
#endif

#if HAS_SERIAL_1
extern volatile struct ao_fifo	ao_serial1_rx_fifo;
extern volatile struct ao_fifo	ao_serial1_tx_fifo;

char
ao_serial1_getchar(void);

int
_ao_serial1_pollchar(void);

uint8_t
_ao_serial1_sleep_for(uint16_t timeout);

void
ao_serial1_putchar(char c);

void
ao_serial1_drain(void);

void
ao_serial1_set_speed(uint8_t speed);
#endif

#if HAS_SERIAL_2
extern volatile struct ao_fifo	ao_serial2_rx_fifo;
extern volatile struct ao_fifo	ao_serial2_tx_fifo;

char
ao_serial2_getchar(void);

int
_ao_serial2_pollchar(void);

uint8_t
_ao_serial2_sleep_for(uint16_t timeout);

void
ao_serial2_putchar(char c);

void
ao_serial2_drain(void);

void
ao_serial2_set_speed(uint8_t speed);
#endif

#if HAS_SERIAL_3
extern volatile struct ao_fifo	ao_serial3_rx_fifo;
extern volatile struct ao_fifo	ao_serial3_tx_fifo;

char
ao_serial3_getchar(void);

int
_ao_serial3_pollchar(void);

uint8_t
_ao_serial3_sleep_for(uint16_t timeout);

void
ao_serial3_putchar(char c);

void
ao_serial3_drain(void);

void
ao_serial3_set_speed(uint8_t speed);
#endif

void
ao_serial_init(void);

#endif /* _AO_SERIAL_H_ */
