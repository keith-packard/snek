/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
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

#ifndef LED_0_INVERT
#define LED_0_INVERT 0
#endif
#ifndef LED_1_INVERT
#define LED_1_INVERT 0
#endif
#ifndef LED_2_INVERT
#define LED_2_INVERT 0
#endif
#ifndef LED_3_INVERT
#define LED_3_INVERT 0
#endif
#ifndef LED_4_INVERT
#define LED_4_INVERT 0
#endif
#ifndef LED_5_INVERT
#define LED_5_INVERT 0
#endif
#ifndef LED_6_INVERT
#define LED_6_INVERT 0
#endif
#ifndef LED_7_INVERT
#define LED_7_INVERT 0
#endif
#ifndef LED_8_INVERT
#define LED_8_INVERT 0
#endif
#ifndef LED_9_INVERT
#define LED_9_INVERT 0
#endif
#ifndef LED_10_INVERT
#define LED_10_INVERT 0
#endif
#ifndef LED_11_INVERT
#define LED_11_INVERT 0
#endif
#ifndef LED_12_INVERT
#define LED_12_INVERT 0
#endif
#ifndef LED_13_INVERT
#define LED_13_INVERT 0
#endif
#ifndef LED_14_INVERT
#define LED_14_INVERT 0
#endif
#ifndef LED_15_INVERT
#define LED_15_INVERT 0
#endif
#ifndef LED_16_INVERT
#define LED_16_INVERT 0
#endif
#ifndef LED_17_INVERT
#define LED_17_INVERT 0
#endif
#ifndef LED_18_INVERT
#define LED_18_INVERT 0
#endif
#ifndef LED_19_INVERT
#define LED_19_INVERT 0
#endif
#ifndef LED_20_INVERT
#define LED_20_INVERT 0
#endif
#ifndef LED_21_INVERT
#define LED_21_INVERT 0
#endif
#ifndef LED_22_INVERT
#define LED_22_INVERT 0
#endif
#ifndef LED_23_INVERT
#define LED_23_INVERT 0
#endif
#ifndef LED_24_INVERT
#define LED_24_INVERT 0
#endif
#ifndef LED_25_INVERT
#define LED_25_INVERT 0
#endif
#ifndef LED_26_INVERT
#define LED_26_INVERT 0
#endif
#ifndef LED_27_INVERT
#define LED_27_INVERT 0
#endif
#ifndef LED_28_INVERT
#define LED_28_INVERT 0
#endif
#ifndef LED_29_INVERT
#define LED_29_INVERT 0
#endif
#ifndef LED_30_INVERT
#define LED_30_INVERT 0
#endif
#ifndef LED_31_INVERT
#define LED_31_INVERT 0
#endif

#ifdef LEDS_AVAILABLE
static const struct {
	void		*port;
	uint8_t		pin;
	uint8_t		invert;
} ao_leds[] = {
#ifdef LED_0_PORT
    [0] { LED_0_PORT, LED_0_PIN, LED_0_INVERT },
#endif
#ifdef LED_1_PORT
    [1] { LED_1_PORT, LED_1_PIN, LED_1_INVERT },
#endif
#ifdef LED_2_PORT
    [2] { LED_2_PORT, LED_2_PIN, LED_2_INVERT },
#endif
#ifdef LED_3_PORT
    [3] { LED_3_PORT, LED_3_PIN, LED_3_INVERT },
#endif
#ifdef LED_4_PORT
    [4] { LED_4_PORT, LED_4_PIN, LED_4_INVERT },
#endif
#ifdef LED_5_PORT
    [5] { LED_5_PORT, LED_5_PIN, LED_5_INVERT },
#endif
#ifdef LED_6_PORT
    [6] { LED_6_PORT, LED_6_PIN, LED_6_INVERT },
#endif
#ifdef LED_7_PORT
    [7] { LED_7_PORT, LED_7_PIN, LED_7_INVERT },
#endif
#ifdef LED_8_PORT
    [8] { LED_8_PORT, LED_8_PIN, LED_8_INVERT },
#endif
#ifdef LED_9_PORT
    [9] { LED_9_PORT, LED_9_PIN, LED_9_INVERT },
#endif
#ifdef LED_10_PORT
    [10] { LED_10_PORT, LED_10_PIN, LED_10_INVERT },
#endif
#ifdef LED_11_PORT
    [11] { LED_11_PORT, LED_11_PIN, LED_11_INVERT },
#endif
#ifdef LED_12_PORT
    [12] { LED_12_PORT, LED_12_PIN, LED_12_INVERT },
#endif
#ifdef LED_13_PORT
    [13] { LED_13_PORT, LED_13_PIN, LED_13_INVERT },
#endif
#ifdef LED_14_PORT
    [14] { LED_14_PORT, LED_14_PIN, LED_14_INVERT },
#endif
#ifdef LED_15_PORT
    [15] { LED_15_PORT, LED_15_PIN, LED_15_INVERT },
#endif
#ifdef LED_16_PORT
    [16] { LED_16_PORT, LED_16_PIN, LED_16_INVERT },
#endif
#ifdef LED_17_PORT
    [17] { LED_17_PORT, LED_17_PIN, LED_17_INVERT },
#endif
#ifdef LED_18_PORT
    [18] { LED_18_PORT, LED_18_PIN, LED_18_INVERT },
#endif
#ifdef LED_19_PORT
    [19] { LED_19_PORT, LED_19_PIN, LED_19_INVERT },
#endif
#ifdef LED_20_PORT
    [20] { LED_20_PORT, LED_20_PIN, LED_20_INVERT },
#endif
#ifdef LED_21_PORT
    [21] { LED_21_PORT, LED_21_PIN, LED_21_INVERT },
#endif
#ifdef LED_22_PORT
    [22] { LED_22_PORT, LED_22_PIN, LED_22_INVERT },
#endif
#ifdef LED_23_PORT
    [23] { LED_23_PORT, LED_23_PIN, LED_23_INVERT },
#endif
#ifdef LED_24_PORT
    [24] { LED_24_PORT, LED_24_PIN, LED_24_INVERT },
#endif
#ifdef LED_25_PORT
    [25] { LED_25_PORT, LED_25_PIN, LED_25_INVERT },
#endif
#ifdef LED_26_PORT
    [26] { LED_26_PORT, LED_26_PIN, LED_26_INVERT },
#endif
#ifdef LED_27_PORT
    [27] { LED_27_PORT, LED_27_PIN, LED_27_INVERT },
#endif
#ifdef LED_28_PORT
    [28] { LED_28_PORT, LED_28_PIN, LED_28_INVERT },
#endif
#ifdef LED_29_PORT
    [29] { LED_29_PORT, LED_29_PIN, LED_29_INVERT },
#endif
#ifdef LED_30_PORT
    [30] { LED_30_PORT, LED_30_PIN, LED_30_INVERT },
#endif
#ifdef LED_31_PORT
    [31] { LED_31_PORT, LED_31_PIN, LED_31_INVERT },
#endif
};
#define N_LED	(sizeof (ao_leds)/sizeof(ao_leds[0]))

void
ao_led_on(AO_LED_TYPE colors)
{
	AO_LED_TYPE i;
	for (i = 0; i < N_LED; i++)
		if (colors & (1 << i))
			ao_gpio_set(ao_leds[i].port, ao_leds[i].pin, !ao_leds[i].invert);
}

void
ao_led_off(AO_LED_TYPE colors)
{
	AO_LED_TYPE i;
	for (i = 0; i < N_LED; i++)
		if (colors & (1 << i))
			ao_gpio_set(ao_leds[i].port, ao_leds[i].pin, ao_leds[i].invert);
}

void
ao_led_init(void)
{
	AO_LED_TYPE	bit;

	for (bit = 0; bit < N_LED; bit++)
		ao_enable_output(ao_leds[bit].port, ao_leds[bit].pin, 0);
}
#endif
