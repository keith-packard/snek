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

#include <ao.h>
#include <ao-tc-samd21.h>

void
ao_tc_set(struct samd21_tc *tc, uint8_t channel, uint32_t value)
{
	tc->mode_16.cc[channel] = value;
}

static void
ao_tc_init(struct samd21_tc *tc, uint32_t apbcmask)
{
	samd21_pm.apbcmask |= apbcmask;

	/* Reset the device */
	tc->ctrla = (1 << SAMD21_TC_CTRLA_SWRST);

	while ((tc->ctrla & (1 << SAMD21_TC_CTRLA_SWRST)) != 0 ||
	       (tc->status & (1 << SAMD21_TC_STATUS_SYNCBUSY)) != 0)
		;

	tc->ctrla = ((SAMD21_TC_CTRLA_PRESCSYNC_GCLK << SAMD21_TC_CTRLA_PRESCSYNC) |
		     (SAMD21_TC_CTRLA_PRESCALER_DIV1 << SAMD21_TC_CTRLA_PRESCALER) |
		     (SAMD21_TC_CTRLA_WAVEGEN_NPWM << SAMD21_TC_CTRLA_WAVEGEN) |
		     (SAMD21_TC_CTRLA_MODE_COUNT16) |
		     (1 << SAMD21_TC_CTRLA_ENABLE));
}

void
ao_tc_samd21_init(void)
{
	/* SAMD21G18 has only TC3-TC5 */
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_TCC2_TC3);
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_TC4_TC5);

	ao_tc_init(&samd21_tc3, 1 << SAMD21_PM_APBCMASK_TC3);
	ao_tc_init(&samd21_tc4, 1 << SAMD21_PM_APBCMASK_TC4);
	ao_tc_init(&samd21_tc5, 1 << SAMD21_PM_APBCMASK_TC5);
}
