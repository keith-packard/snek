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

#ifndef _AO_LED_H_
#define _AO_LED_H_

/*
 * ao_led.c
 */

#define AO_LED_NONE	0

#ifndef AO_LED_TYPE
#define AO_LED_TYPE uint32_t
#endif

/* Turn on the specified LEDs */
void
ao_led_on(AO_LED_TYPE colors);

/* Turn off the specified LEDs */
void
ao_led_off(AO_LED_TYPE colors);

/* Set all of the LEDs to the specified state */
void
ao_led_set(AO_LED_TYPE colors);

/* Set all LEDs in 'mask' to the specified state */
void
ao_led_set_mask(AO_LED_TYPE colors, AO_LED_TYPE mask);

/* Toggle the specified LEDs */
void
ao_led_toggle(AO_LED_TYPE colors);

/* Turn on the specified LEDs for the indicated interval */
void
ao_led_for(AO_LED_TYPE colors, AO_TICK_TYPE ticks);

/* Initialize the LEDs */
void
ao_led_init(void);

#ifdef LED_0_PORT
#define AO_LED_0 (1 << 0)
#else
#define AO_LED_0 0
#endif
#ifdef LED_1_PORT
#define AO_LED_1 (1 << 1)
#else
#define AO_LED_1 0
#endif
#ifdef LED_2_PORT
#define AO_LED_2 (1 << 2)
#else
#define AO_LED_2 0
#endif
#ifdef LED_3_PORT
#define AO_LED_3 (1 << 3)
#else
#define AO_LED_3 0
#endif
#ifdef LED_4_PORT
#define AO_LED_4 (1 << 4)
#else
#define AO_LED_4 0
#endif
#ifdef LED_5_PORT
#define AO_LED_5 (1 << 5)
#else
#define AO_LED_5 0
#endif
#ifdef LED_6_PORT
#define AO_LED_6 (1 << 6)
#else
#define AO_LED_6 0
#endif
#ifdef LED_7_PORT
#define AO_LED_7 (1 << 7)
#else
#define AO_LED_7 0
#endif
#ifdef LED_8_PORT
#define AO_LED_8 (1 << 8)
#else
#define AO_LED_8 0
#endif
#ifdef LED_9_PORT
#define AO_LED_9 (1 << 9)
#else
#define AO_LED_9 0
#endif
#ifdef LED_10_PORT
#define AO_LED_10 (1 << 10)
#else
#define AO_LED_10 0
#endif
#ifdef LED_11_PORT
#define AO_LED_11 (1 << 11)
#else
#define AO_LED_11 0
#endif
#ifdef LED_12_PORT
#define AO_LED_12 (1 << 12)
#else
#define AO_LED_12 0
#endif
#ifdef LED_13_PORT
#define AO_LED_13 (1 << 13)
#else
#define AO_LED_13 0
#endif
#ifdef LED_14_PORT
#define AO_LED_14 (1 << 14)
#else
#define AO_LED_14 0
#endif
#ifdef LED_15_PORT
#define AO_LED_15 (1 << 15)
#else
#define AO_LED_15 0
#endif
#ifdef LED_16_PORT
#define AO_LED_16 (1 << 16)
#else
#define AO_LED_16 0
#endif
#ifdef LED_17_PORT
#define AO_LED_17 (1 << 17)
#else
#define AO_LED_17 0
#endif
#ifdef LED_18_PORT
#define AO_LED_18 (1 << 18)
#else
#define AO_LED_18 0
#endif
#ifdef LED_19_PORT
#define AO_LED_19 (1 << 19)
#else
#define AO_LED_19 0
#endif
#ifdef LED_20_PORT
#define AO_LED_20 (1 << 20)
#else
#define AO_LED_20 0
#endif
#ifdef LED_21_PORT
#define AO_LED_21 (1 << 21)
#else
#define AO_LED_21 0
#endif
#ifdef LED_22_PORT
#define AO_LED_22 (1 << 22)
#else
#define AO_LED_22 0
#endif
#ifdef LED_23_PORT
#define AO_LED_23 (1 << 23)
#else
#define AO_LED_23 0
#endif
#ifdef LED_24_PORT
#define AO_LED_24 (1 << 24)
#else
#define AO_LED_24 0
#endif
#ifdef LED_25_PORT
#define AO_LED_25 (1 << 25)
#else
#define AO_LED_25 0
#endif
#ifdef LED_26_PORT
#define AO_LED_26 (1 << 26)
#else
#define AO_LED_26 0
#endif
#ifdef LED_27_PORT
#define AO_LED_27 (1 << 27)
#else
#define AO_LED_27 0
#endif
#ifdef LED_28_PORT
#define AO_LED_28 (1 << 28)
#else
#define AO_LED_28 0
#endif
#ifdef LED_29_PORT
#define AO_LED_29 (1 << 29)
#else
#define AO_LED_29 0
#endif
#ifdef LED_30_PORT
#define AO_LED_30 (1 << 30)
#else
#define AO_LED_30 0
#endif
#ifdef LED_31_PORT
#define AO_LED_31 (1 << 31)
#else
#define AO_LED_31 0
#endif

#define AO_LEDS_AVAILABLE (AO_LED_0 |		\
			   AO_LED_1 |		\
			   AO_LED_2 |		\
			   AO_LED_3 |		\
			   AO_LED_4 |		\
			   AO_LED_5 |		\
			   AO_LED_6 |		\
			   AO_LED_7 |		\
			   AO_LED_8 |		\
			   AO_LED_9 |		\
			   AO_LED_10 |		\
			   AO_LED_11 |		\
			   AO_LED_12 |		\
			   AO_LED_13 |		\
			   AO_LED_14 |		\
			   AO_LED_15 |		\
			   AO_LED_16 |		\
			   AO_LED_17 |		\
			   AO_LED_18 |		\
			   AO_LED_19 |		\
			   AO_LED_20 |		\
			   AO_LED_21 |		\
			   AO_LED_22 |		\
			   AO_LED_23 |		\
			   AO_LED_24 |		\
			   AO_LED_25 |		\
			   AO_LED_26 |		\
			   AO_LED_27 |		\
			   AO_LED_28 |		\
			   AO_LED_29 |		\
			   AO_LED_30 |		\
			   AO_LED_31)

#ifndef LEDS_AVAILABLE
#define LEDS_AVAILABLE AO_LEDS_AVAILABLE
#endif

#endif /* _AO_LED_H_ */
