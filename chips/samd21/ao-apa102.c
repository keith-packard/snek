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

#include <ao.h>
#include <ao-snek.h>

#define bit(v)	do {				\
		port_c->outtgl = (1 << pin_c);	\
		ao_arch_nop(); \
		ao_gpio_set(port_d, pin_d, v);	\
		ao_arch_nop(); \
		port_c->outtgl = (1 << pin_c);	\
		ao_arch_nop(); \
	} while (0)

#define byte(v)	do {				\
		uint8_t _bit_ = 0x80;		\
		while (_bit_) {			\
			bit(!!((v) & _bit_));	\
			_bit_ >>= 1;		\
		}				\
	} while(0)

#define repeat(v,c) do {			\
		uint8_t _i_;			\
		for (_i_ = 0; _i_ < (c); _i_++) \
			bit(v);			\
	} while (0)

void
ao_snek_apa102_write(void *gpio_d, uint8_t pin_d,
		     void *gpio_c, uint8_t pin_c,
		     int npixel,
		     struct snek_neopixel *pixels)
{
	struct samd21_port *port_d = gpio_d;
	struct samd21_port *port_c = gpio_c;

	ao_gpio_set(port_c, pin_c, 1);
	int i;
	for (i = 0; i < 32; i++)
		ao_arch_nop();
	repeat(0, 32);
	while (npixel--) {
		byte(0xff);
		byte(pixels->b);
		byte(pixels->g);
		byte(pixels->r);
	}
	repeat(1, 32);
}
