/*
 * Copyright © 2020 Keith Packard <keithp@keithp.com>
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

#include <snek.h>
#include <ao.h>
#include <ao-dac-samd21.h>
#include <ao-tcc-samd21.h>

/* Max DAC output value. We're using left-adjusted values */
#define SNEK_DAC_MAX		65535

#ifdef SNEK_SAMD21_DAC_TIMER
/*
 * If there's a timer available, we can use that
 * to implement the 'tone' function
 */

#include "sine.h"

#define NSINE	(sizeof(sine) / sizeof(sine[0]))

static uint16_t current_power;
static uint16_t power;
static uint32_t	phase;
static uint32_t phase_step;
static volatile bool dac_running;

#define _paste2(x,y)	x ## y
#define _paste3(x,y,z)	x ## y ## z
#define paste2(x,y)	_paste2(x,y)
#define paste3(x,y,z)	_paste3(x,y,z)
#define SAMD21_TCC	paste2(samd21_tcc, SNEK_SAMD21_DAC_TIMER)
#define SAMD21_TCC_ISR	paste3(samd21_tcc, SNEK_SAMD21_DAC_TIMER, _isr)

#define AO_DAC_RATE	24000

#define UINT_TO_FIXED(u)	((uint32_t) (u) << 16)
#define FIXED_TO_UINT(u)	((u) >> 16)

void
SAMD21_TCC_ISR(void)
{
	uint32_t intflag = SAMD21_TCC.intflag;
	SAMD21_TCC.intflag = intflag;
	if (intflag & (1 << SAMD21_TCC_INTFLAG_OVF)) {
		if (phase_step) {
			samd21_dac.data = ((uint32_t) sine[FIXED_TO_UINT(phase)] * current_power) >> 16;
			if ((phase += phase_step) >= UINT_TO_FIXED(NSINE)) {
				phase -= UINT_TO_FIXED(NSINE);

				current_power = power;

				/* Stop output at zero crossing when no longer outputing tone */
				if (!dac_running) {
					phase_step = 0;
					phase = 0;
					SAMD21_TCC.intenclr = (1 << SAMD21_TCC_INTFLAG_OVF);
				}
			}
		}
	}
}

void
ao_dac_set_hz(float hz)
{
	/* samples/second = AC_DAC_RATE
	 *
	 * cycles/second = hz
	 *
	 * samples/cycle = AC_DAC_RATE / hz
	 *
	 * step/cycle = 256
	 *
	 * step/sample = step/cycle * cycle/samples
	 *	       = TWO_PI * hz / AC_DAC_RATE;
	 */
	uint32_t new_phase_step = (float) UINT_TO_FIXED(NSINE) * hz / (float) AO_DAC_RATE;
	ao_arch_critical(
		if (new_phase_step) {
			dac_running = true;
			phase_step = new_phase_step;
			SAMD21_TCC.intenset = (1 << SAMD21_TCC_INTFLAG_OVF);
		} else {
			dac_running = false;
		});
}

static void
ao_dac_timer_init(void)
{
	/* Adjust timer to interrupt once per sample period */
	SAMD21_TCC.per = AO_HCLK / AO_DAC_RATE;

	/* Enable timer interrupts */
	samd21_nvic_set_enable(paste3(SAMD21_NVIC_ISR_TCC, SNEK_SAMD21_DAC_TIMER, _POS));
	samd21_nvic_set_priority(paste3(SAMD21_NVIC_ISR_TCC, SNEK_SAMD21_DAC_TIMER, _POS), 3);
}
#else
#define ao_dac_timer_init()
#endif

static void
ao_dac_sync(void)
{
	while (samd21_dac.status & (1 << SAMD21_DAC_STATUS_SYNCBUSY))
		;
}

void
ao_dac_set(uint16_t new_power)
{
#if SNEK_DAC_MAX != SNEK_PWM_MAX
	new_power = (uint16_t) ((uint32_t) new_power * SNEK_DAC_MAX) / SNEK_PWM_MAX;
#endif

	ao_arch_critical(
#ifdef SNEK_SAMD21_DAC_TIMER
		power = new_power;
		/*
		 * When not generating a tone, just set the DAC
		 * output to the requested level
		 */
		if (!phase_step) {
			current_power = new_power;
			samd21_dac.data = new_power;
		}
#else
		samd21_dac.data = new_power;
#endif
		);
}

void
ao_dac_init(void)
{
	/* supply a clock */
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_DAC);

	/* enable the device */
	samd21_pm.apbcmask |= (1 << SAMD21_PM_APBCMASK_DAC);

	/* reset */
	samd21_dac.ctrla = (1 << SAMD21_DAC_CTRLA_SWRST);

	while ((samd21_dac.ctrla & (1 << SAMD21_DAC_CTRLA_SWRST)) != 0 ||
	       (samd21_dac.status & (1 << SAMD21_DAC_STATUS_SYNCBUSY)) != 0)
		ao_arch_nop();

	/* Configure using VDD as reference */
	samd21_dac.ctrlb = ((1 << SAMD21_DAC_CTRLB_EOEN) |
			    (0 << SAMD21_DAC_CTRLB_IOEN) |
			    (1 << SAMD21_DAC_CTRLB_LEFTADJ) |
			    (0 << SAMD21_DAC_CTRLB_VPD) |
			    (1 << SAMD21_DAC_CTRLB_BDWP) |
			    (SAMD21_DAC_CTRLB_REFSEL_VDDANA << SAMD21_DAC_CTRLB_REFSEL));

	ao_dac_sync();

	samd21_dac.ctrla = (1 << SAMD21_DAC_CTRLA_ENABLE);

	ao_dac_sync();

	ao_dac_timer_init();
}
