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

static const struct {
	void		*port;
	uint16_t	pin;
} ao_leds[] = {
#ifdef LED_0_PORT
    [0] { LED_0_PORT, LED_0_PIN },
#endif
#ifdef LED_1_PORT
    [1] { LED_1_PORT, LED_1_PIN },
#endif
#ifdef LED_2_PORT
    [2] { LED_2_PORT, LED_2_PIN },
#endif
#ifdef LED_3_PORT
    [3] { LED_3_PORT, LED_3_PIN },
#endif
#ifdef LED_4_PORT
    [4] { LED_4_PORT, LED_4_PIN },
#endif
#ifdef LED_5_PORT
    [5] { LED_5_PORT, LED_5_PIN },
#endif
#ifdef LED_6_PORT
    [6] { LED_6_PORT, LED_6_PIN },
#endif
#ifdef LED_7_PORT
    [7] { LED_7_PORT, LED_7_PIN },
#endif
#ifdef LED_8_PORT
    [8] { LED_8_PORT, LED_8_PIN },
#endif
#ifdef LED_9_PORT
    [9] { LED_9_PORT, LED_9_PIN },
#endif
#ifdef LED_10_PORT
    [10] { LED_10_PORT, LED_10_PIN },
#endif
#ifdef LED_11_PORT
    [11] { LED_11_PORT, LED_11_PIN },
#endif
#ifdef LED_12_PORT
    [12] { LED_12_PORT, LED_12_PIN },
#endif
#ifdef LED_13_PORT
    [13] { LED_13_PORT, LED_13_PIN },
#endif
#ifdef LED_14_PORT
    [14] { LED_14_PORT, LED_14_PIN },
#endif
#ifdef LED_15_PORT
    [15] { LED_15_PORT, LED_15_PIN },
#endif
#ifdef LED_16_PORT
    [16] { LED_16_PORT, LED_16_PIN },
#endif
#ifdef LED_17_PORT
    [17] { LED_17_PORT, LED_17_PIN },
#endif
#ifdef LED_18_PORT
    [18] { LED_18_PORT, LED_18_PIN },
#endif
#ifdef LED_19_PORT
    [19] { LED_19_PORT, LED_19_PIN },
#endif
#ifdef LED_20_PORT
    [20] { LED_20_PORT, LED_20_PIN },
#endif
#ifdef LED_21_PORT
    [21] { LED_21_PORT, LED_21_PIN },
#endif
#ifdef LED_22_PORT
    [22] { LED_22_PORT, LED_22_PIN },
#endif
#ifdef LED_23_PORT
    [23] { LED_23_PORT, LED_23_PIN },
#endif
#ifdef LED_24_PORT
    [24] { LED_24_PORT, LED_24_PIN },
#endif
#ifdef LED_25_PORT
    [25] { LED_25_PORT, LED_25_PIN },
#endif
#ifdef LED_26_PORT
    [26] { LED_26_PORT, LED_26_PIN },
#endif
#ifdef LED_27_PORT
    [27] { LED_27_PORT, LED_27_PIN },
#endif
#ifdef LED_28_PORT
    [28] { LED_28_PORT, LED_28_PIN },
#endif
#ifdef LED_29_PORT
    [29] { LED_29_PORT, LED_29_PIN },
#endif
#ifdef LED_30_PORT
    [30] { LED_30_PORT, LED_30_PIN },
#endif
#ifdef LED_31_PORT
    [31] { LED_31_PORT, LED_31_PIN },
#endif
};
#define N_LED	(sizeof (ao_leds)/sizeof(ao_leds[0]))

void
ao_led_on(AO_LED_TYPE colors)
{
	AO_LED_TYPE i;
	for (i = 0; i < N_LED; i++)
		if (colors & (1 << i))
			ao_gpio_set(ao_leds[i].port, ao_leds[i].pin, 1);
}

void
ao_led_off(AO_LED_TYPE colors)
{
	AO_LED_TYPE i;
	for (i = 0; i < N_LED; i++)
		if (colors & (1 << i))
			ao_gpio_set(ao_leds[i].port, ao_leds[i].pin, 0);
}

void
ao_led_set(AO_LED_TYPE colors)
{
	AO_LED_TYPE i;
	for (i = 0; i < N_LED; i++)
		ao_gpio_set(ao_leds[i].port, ao_leds[i].pin, (colors >> i) & 1);
}

void
ao_led_toggle(AO_LED_TYPE colors)
{
	AO_LED_TYPE i;
	for (i = 0; i < N_LED; i++)
		if (colors & (1 << i))
			ao_gpio_set(ao_leds[i].port, ao_leds[i].pin, ~ao_gpio_get(ao_leds[i].port, ao_leds[i].pin));
}

void
ao_led_for(AO_LED_TYPE colors, AO_TICK_TYPE ticks) 
{
	ao_led_on(colors);
	ao_delay(ticks);
	ao_led_off(colors);
}

void
ao_led_init(void)
{
	AO_LED_TYPE	bit;

	for (bit = 0; bit < N_LED; bit++)
		ao_enable_output(ao_leds[bit].port, ao_leds[bit].pin, 0);
}
