/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <ao.h>
#include <ao_tcc_samd21.h>

void
ao_tcc_set(struct samd21_tcc *tcc, uint8_t channel, uint32_t value)
{
	tcc->cc[channel] = value;
}

static void
ao_tcc_init(struct samd21_tcc *tcc, uint32_t apbcmask)
{
	samd21_pm.apbcmask |= apbcmask;

	/* Reset the device */
	tcc->ctrla = (1 << SAMD21_TCC_CTRLA_SWRST);

	while ((tcc->ctrla & (1 << SAMD21_TCC_CTRLA_SWRST)) != 0 ||
	       (tcc->syncbusy & (1 << SAMD21_TCC_SYNCBUSY_SWRST)) != 0)
		;

	tcc->per = AO_TCC_PERIOD - 1;

#if 0
	tcc->evctrl = ((1 << SAMD21_TCC_EVCTRL_MCEO(0)) |
		       (1 << SAMD21_TCC_EVCTRL_MCEO(1)) |
		       (1 << SAMD21_TCC_EVCTRL_MCEO(2)) |
		       (1 << SAMD21_TCC_EVCTRL_MCEO(3)));
#endif

	tcc->wave = ((SAMD21_TCC_WAVE_WAVEGEN_NPWM << SAMD21_TCC_WAVE_WAVEGEN) |
		     (0 << SAMD21_TCC_WAVE_RAMP) |
		     (0 << SAMD21_TCC_WAVE_CIPEREN) |
		     (0 << SAMD21_TCC_WAVE_CCCEN(0)) |
		     (0 << SAMD21_TCC_WAVE_CCCEN(1)) |
		     (0 << SAMD21_TCC_WAVE_CCCEN(2)) |
		     (0 << SAMD21_TCC_WAVE_CCCEN(3)) |
		     (0 << SAMD21_TCC_WAVE_POL(0)) |
		     (0 << SAMD21_TCC_WAVE_POL(1)) |
		     (0 << SAMD21_TCC_WAVE_POL(1)) |
		     (0 << SAMD21_TCC_WAVE_POL(3)) |
		     (0 << SAMD21_TCC_WAVE_SWAP(0)) |
		     (0 << SAMD21_TCC_WAVE_SWAP(1)) |
		     (0 << SAMD21_TCC_WAVE_SWAP(1)) |
		     (0 << SAMD21_TCC_WAVE_SWAP(3)));

	tcc->ctrla = (1 << SAMD21_TCC_CTRLA_ENABLE);
}

void
ao_tcc_samd21_init(void)
{
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_TCC0_TCC1);
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_TCC2_TC3);

	ao_tcc_init(&samd21_tcc0, 1 << SAMD21_PM_APBCMASK_TCC0);
	ao_tcc_init(&samd21_tcc1, 1 << SAMD21_PM_APBCMASK_TCC1);
	ao_tcc_init(&samd21_tcc2, 1 << SAMD21_PM_APBCMASK_TCC2);
}
