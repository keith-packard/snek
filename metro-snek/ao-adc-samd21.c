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
#include <ao-adc-samd21.h>

static void
ao_adc_sync(void)
{
	while (samd21_adc.status & (1 << SAMD21_ADC_STATUS_SYNCBUSY))
		;
}

uint16_t
ao_adc_read(uint8_t channel)
{
	ao_adc_sync();
	samd21_adc.inputctrl = ((channel << SAMD21_ADC_INPUTCTRL_MUXPOS) |
				(SAMD21_ADC_INPUTCTRL_MUXNEG_GND << SAMD21_ADC_INPUTCTRL_MUXNEG) |
				(0 << SAMD21_ADC_INPUTCTRL_INPUTSCAN) |
				(0 << SAMD21_ADC_INPUTCTRL_INPUTOFFSET) |
				(SAMD21_ADC_INPUTCTRL_GAIN_DIV2 << SAMD21_ADC_INPUTCTRL_GAIN));
	ao_adc_sync();
	samd21_adc.swtrig = (1 << SAMD21_ADC_SWTRIG_START);
	while ((samd21_adc.intflag & (1 << SAMD21_ADC_INTFLAG_RESRDY)) == 0)
		;
	uint16_t	result = samd21_adc.result;
	return result;
}

void
ao_adc_init(void)
{
	/* supply a clock */
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_ADC);

	/* enable the device */
	samd21_pm.apbcmask |= (1 << SAMD21_PM_APBCMASK_ADC);

	/* Reset */
	samd21_adc.ctrla = (1 << SAMD21_ADC_CTRLA_SWRST);

	while ((samd21_adc.ctrla & (1 << SAMD21_ADC_CTRLA_SWRST)) != 0 ||
	       (samd21_adc.status & (1 << SAMD21_ADC_STATUS_SYNCBUSY)) != 0)
		;

	/* Load ADC calibration values */
	uint32_t b = (samd21_aux1.calibration >> SAMD21_AUX1_CALIBRATION_ADC_BIASCAL) & SAMD21_AUX1_CALIBRATION_ADC_BIASCAL_MASK;
	uint32_t l = (samd21_aux1.calibration >> SAMD21_AUX1_CALIBRATION_ADC_LINEARITY) & SAMD21_AUX1_CALIBRATION_ADC_LINEARITY_MASK;

	samd21_adc.calib = ((b << SAMD21_ADC_CALIB_BIAS_CAL) |
			    (l << SAMD21_ADC_CALIB_LINEARITY_CAL));


	samd21_adc.ctrlb = ((0 << SAMD21_ADC_CTRLB_DIFFMODE) |
			    (0 << SAMD21_ADC_CTRLB_LEFTADJ) |
			    (0 << SAMD21_ADC_CTRLB_FREERUN) |
			    (0 << SAMD21_ADC_CTRLB_CORREN) |
			    (SAMD21_ADC_CTRLB_RESSEL_12BIT << SAMD21_ADC_CTRLB_RESSEL) |
			    (SAMD21_ADC_CTRLB_PRESCALER_DIV512 << SAMD21_ADC_CTRLB_PRESCALER));
	samd21_adc.sampctrl = 0x3f;
	samd21_adc.refctrl = (SAMD21_ADC_REFCTRL_REFSEL_INTVCC1 << SAMD21_ADC_REFCTRL_REFSEL);

	samd21_adc.ctrla = (1 << SAMD21_ADC_CTRLA_ENABLE);

	/* Discard the first sample */
	(void) ao_adc_read(0);
}
