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

#if !HAS_BEEP
#define ao_beep(x)
#endif
#if !LEDS_AVAILABLE
#define ao_led_on(x)
#define ao_led_off(x)
#endif

#ifndef AO_LED_PANIC
#define AO_LED_PANIC	AO_LED_RED
#endif

static void
ao_panic_delay(uint8_t n)
{
	uint8_t	i = 0, j = 0;

	while (n--) {
#ifdef AO_PANIC_DELAY_SCALE
	uint8_t s = AO_PANIC_DELAY_SCALE;
	while (s--)
#endif
		while (--j)
			while (--i)
				ao_arch_nop();
	}
}

void
ao_panic(uint8_t reason)
{
	uint8_t	n;

	ao_arch_block_interrupts();
	for (;;) {
		ao_panic_delay(20);
#if HAS_BEEP
		for (n = 0; n < 5; n++) {
			ao_beep(AO_BEEP_HIGH_PANIC);
			ao_panic_delay(1);
			ao_beep(AO_BEEP_LOW_PANIC);
			ao_panic_delay(1);
		}
		ao_beep(AO_BEEP_OFF);
		ao_panic_delay(2);
#endif

#ifdef SDCC
#pragma disable_warning 126
#endif
		if (reason & 0x40) {
			ao_led_on(AO_LED_PANIC);
			ao_beep(AO_BEEP_HIGH_PANIC);
			ao_panic_delay(40);
			ao_led_off(AO_LED_PANIC);
			ao_beep(AO_BEEP_OFF);
			ao_panic_delay(10);
		}
		for (n = 0; n < (reason & 0x3f); n++) {
			ao_led_on(AO_LED_PANIC);
			ao_beep(AO_BEEP_MID_PANIC);
			ao_panic_delay(10);
			ao_led_off(AO_LED_PANIC);
			ao_beep(AO_BEEP_OFF);
			ao_panic_delay(10);
		}
	}
}
