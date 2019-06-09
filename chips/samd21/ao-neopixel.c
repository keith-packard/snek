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

void
ao_snek_neopixel_write(void *gpio, uint8_t pin, int npixel, struct snek_neopixel *pixels)
{
	volatile uint32_t *outtgl = &(((struct samd21_port *) gpio)->outtgl);
	uint32_t value = ((uint32_t) 1 << pin);

	while (npixel--) {
		int32_t p = pixels->p;
		uint8_t bit;
		pixels++;

		ao_arch_block_interrupts();
		for (bit = 0; bit < 24; bit++) {
			*outtgl = value;
			ao_arch_nop();
			ao_arch_nop();
			ao_arch_nop();
			ao_arch_nop();
			ao_arch_nop();

			ao_arch_nop();
			ao_arch_nop();
			ao_arch_nop();
			if (p < 0) {
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();

				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();

				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();

				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();

				*outtgl = value;
			} else {
				*outtgl = value;
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();

				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();

				ao_arch_nop();
				ao_arch_nop();
				ao_arch_nop();

			}
			ao_arch_nop();
			ao_arch_nop();
			ao_arch_nop();
			ao_arch_nop();
			ao_arch_nop();

			ao_arch_nop();

			p <<= 1;
		}
		ao_arch_release_interrupts();
	}
}
