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

#ifndef _AO_SNEK_H_
#define _AO_SNEK_H_

#include <ao.h>

#define SNEK_PIN_PULL_DOWN	0x01

int
snek_eeprom_getchar(FILE *stream);

void
ao_snek_set_adc(void *gpio, uint8_t pin);

void
ao_snek_clr_adc(void *gpio, uint8_t pin);

uint16_t
ao_snek_get_adc(uint8_t adc);

void
ao_snek_port_init(void);

void
ao_snek(void);

void
ao_snek_reset(void);

void
ao_snek_running(bool running);

#define SNEK_CODE_HOOK_START	ao_snek_running(true);
#define SNEK_CODE_HOOK_STOP	ao_snek_running(false);

struct snek_neopixel {
	union {
		struct {
			uint8_t	_extra, b, r, g;
		};
		uint32_t	p;
	};
};

extern struct snek_neopixel	*snek_neopixels;

#define SNEK_MEM_CACHE_NUM	1
#define SNEK_MEM_CACHE_0	snek_neopixels

void
ao_snek_neopixel_write(void *port, uint8_t pin, int npixel, struct snek_neopixel *pixels);

#endif /* _AO_SNEK_H_ */
