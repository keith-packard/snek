/*
 * Copyright © 2025 Keith Packard <keithp@keithp.com>
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

#ifndef _SNEK_70909_H_
#define _SNEK_70909_H_

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

#define LEGO_ERR	-1
#define LEGO_TIMEOUT	-2
#define LEGO_CHKSUM	-3
#define LEGO_OK		0

#define LEGO_SENSORS	8
#define LEGO_OUTPUTS	8

#define LEGO_BUFSIZE	512

typedef struct {
	uint16_t	value;
	uint16_t	status;
} lego_sensor_t;

typedef struct {
	uint8_t	power;
	uint8_t left;
	uint8_t on;
} lego_output_t;

typedef struct {
	int		fd;
	char		buf[LEGO_BUFSIZE];
	size_t		count;
	size_t		used;
	uint32_t	keepalive;

	FILE		*out;

	pthread_t	thread;
	pthread_mutex_t	mutex;
	bool		running;

	bool		connected;

	lego_sensor_t	sensors[LEGO_SENSORS];
	lego_output_t	outputs[LEGO_OUTPUTS];
	lego_output_t	active[LEGO_OUTPUTS];
	char		name[];
} dacta_t;

int
lego_start(dacta_t *l, char *device);

void
lego_stop(dacta_t *l);

lego_sensor_t
lego_sensor(dacta_t *l, int i);

bool
lego_connected(dacta_t *l);

static inline void lego_nop(dacta_t *l, int p)
{ putc(0x00 | p, l->out); fflush(l->out); }

static inline void lego_port_left(dacta_t *l, int p)
{ putc(0x10 | p, l->out); fflush(l->out); }

static inline void lego_port_right(dacta_t *l, int p)
{ putc(0x18 | p, l->out); fflush(l->out); }

static inline void lego_reverse(dacta_t *l, int p)
{ putc(0x20 | p, l->out); fflush(l->out); }

static inline void lego_port_on(dacta_t *l, int p)
{ putc(0x28 | p, l->out); fflush(l->out); }

static inline void lego_port_off(dacta_t *l, int p)
{ putc(0x30 | p, l->out); fflush(l->out); }

static inline void lego_set_left(dacta_t *l, int p)
{ putc(0x40 | p, l->out); fflush(l->out); }

static inline void lego_set_right(dacta_t *l, int p)
{ putc(0x48 | p, l->out); fflush(l->out); }

static inline void lego_dacta_off(dacta_t *l)
{ putc(0x70, l->out); fflush(l->out); }

static inline void lego_ports_off_steady(dacta_t *l, uint8_t pm)
{ putc(0x80, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_ports_flash(dacta_t *l, uint8_t pm)
{ putc(0x81, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_ports_off(dacta_t *l, uint8_t pm)
{ putc(0x90, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_ports_on(dacta_t *l, uint8_t pm)
{ putc(0x91, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_ports_power0(dacta_t *l, uint8_t pm)
{ putc(0x92, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_ports_left(dacta_t *l, uint8_t pm)
{ putc(0x93, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_ports_right(dacta_t *l, uint8_t pm)
{ putc(0x94, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_ports_reverse(dacta_t *l, uint8_t pm)
{ putc(0x95, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_set_powers(dacta_t *l, int pow, uint8_t pm)
{ putc(0xb0 | pow, l->out); putc(pm, l->out); fflush(l->out); }

static inline void lego_port_on_for(dacta_t *l, int p, int t)
{ putc(0xc0 | p, l->out); putc(t, l->out); fflush(l->out); }

static inline void lego_port_on_cycle(dacta_t *l, int p, int t)
{ putc(0xe8 | p, l->out); putc(t, l->out); fflush(l->out); }

static inline void lego_port_off_cycle(dacta_t *l, int p, int t)
{ putc(0xe0 | p, l->out); putc(t, l->out); fflush(l->out); }

#endif /* _SNEK_70909_H_ */
