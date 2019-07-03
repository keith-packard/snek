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

#ifndef HAS_TICK
#define HAS_TICK 1
#endif

#if HAS_TICK
volatile AO_TICK_TYPE ao_tick_count;

AO_TICK_TYPE
ao_time(void)
{
	return ao_tick_count;
}

uint64_t
ao_time_ns(void)
{
	AO_TICK_TYPE	before, after;
	uint32_t	cvr;

	do {
		before = ao_tick_count;
		cvr = samd21_systick.cvr;
		after = ao_tick_count;
	} while (before != after);

	return (uint64_t) after * (1000000000ULL / AO_HERTZ) +
		(uint64_t) cvr * (1000000000ULL / AO_SYSTICK);
}

#if AO_DATA_ALL
volatile uint8_t	ao_data_interval = 1;
volatile uint8_t	ao_data_count;
#endif

void samd21_systick_isr(void)
{
	if (samd21_systick.csr & (1 << SAMD21_SYSTICK_CSR_COUNTFLAG)) {
		++ao_tick_count;
#if HAS_TASK_QUEUE
		if (ao_task_alarm_tick && (int16_t) (ao_tick_count - ao_task_alarm_tick) >= 0)
			ao_task_check_alarm((uint16_t) ao_tick_count);
#endif
#if AO_DATA_ALL
		if (++ao_data_count == ao_data_interval) {
			ao_data_count = 0;
#if HAS_ADC
#if HAS_FAKE_FLIGHT
			if (ao_fake_flight_active)
				ao_fake_flight_poll();
			else
#endif
				ao_adc_poll();
#endif
#if (AO_DATA_ALL & ~(AO_DATA_ADC))
			ao_wakeup((void *) &ao_data_count);
#endif
		}
#endif
#ifdef AO_TIMER_HOOK
		AO_TIMER_HOOK;
#endif
	}
}

#if HAS_ADC
void
ao_timer_set_adc_interval(uint8_t interval)
{
	ao_arch_critical(
		ao_data_interval = interval;
		ao_data_count = 0;
		);
}
#endif

#define SYSTICK_RELOAD (AO_SYSTICK / 100 - 1)

void
ao_timer_init(void)
{
	samd21_systick.csr = 0;
	samd21_systick.rvr = SYSTICK_RELOAD;
	samd21_systick.cvr = 0;
	samd21_systick.csr = ((1 << SAMD21_SYSTICK_CSR_ENABLE) |
			   (1 << SAMD21_SYSTICK_CSR_TICKINT) |
			   (SAMD21_SYSTICK_CSR_CLKSOURCE_HCLK_8 << SAMD21_SYSTICK_CSR_CLKSOURCE));
}

#endif


void
ao_clock_init(void)
{
	/* Set flash wait state to tolerate 48MHz */
	samd21_nvmctrl.ctrlb |= (1 << SAMD21_NVMCTRL_CTRLB_RWS);

	samd21_pm.apbamask |= ((1 << SAMD21_PM_APBAMASK_GCLK) |
			       (1 << SAMD21_PM_APBAMASK_SYSCTRL));

#ifdef AO_XOSC32K
	/* Enable xosc32k (external 32.768kHz oscillator) */
	samd21_sysctrl.xosc32k = ((6 << SAMD21_SYSCTRL_XOSC32K_STARTUP) |
				  (1 << SAMD21_SYSCTRL_XOSC32K_XTALEN) |
				  (1 << SAMD21_SYSCTRL_XOSC32K_EN32K));

	/* requires separate store */
	samd21_sysctrl.xosc32k |= (1 << SAMD21_SYSCTRL_XOSC32K_ENABLE);

	/* Wait for osc */
	while ((samd21_sysctrl.pclksr & (1 << SAMD21_SYSCTRL_PCLKSR_XOSC32KRDY)) == 0)
		;
#endif
#ifdef AO_XOSC
	/* Enable xosc (external xtal oscillator) */
	samd21_sysctrl.xosc = ((SAMD21_SYSCTRL_XOSC_STARTUP_8192 << SAMD21_SYSCTRL_XOSC_STARTUP) |
			       (1 << SAMD21_SYSCTRL_XOSC_AMPGC) |
			       (SAMD21_SYSCTRL_XOSC_GAIN_16MHz << SAMD21_SYSCTRL_XOSC_GAIN) |
			       (0 << SAMD21_SYSCTRL_XOSC_ONDEMAND) |
			       (0 << SAMD21_SYSCTRL_XOSC_RUNSTDBY) |
			       (1 << SAMD21_SYSCTRL_XOSC_XTALEN));
	samd21_sysctrl.xosc |= ((1 << SAMD21_SYSCTRL_XOSC_ENABLE));

	/* Wait for xosc */
	while ((samd21_sysctrl.pclksr & (1 << SAMD21_SYSCTRL_PCLKSR_XOSCRDY)) == 0)
		;
#endif

	/* Reset gclk */
	samd21_gclk.ctrl = (1 << SAMD21_GCLK_CTRL_SWRST)
		;

	/* Wait for reset to complete */
	while ((samd21_gclk.ctrl & (1 << SAMD21_GCLK_CTRL_SWRST)) &&
	       (samd21_gclk.status & (1 << SAMD21_GCLK_STATUS_SYNCBUSY)))
		;

#define AO_GCLK_SYSCLK	0

#ifdef AO_XOSC32K
#define AO_GCLK_XOSC32K	1

	/*
	 * Use xosc32k as source of gclk generator AO_GCLK_XOSC32K
	 */

	samd21_gclk_gendiv(AO_GCLK_XOSC32K, 1);
	samd21_gclk_genctrl(SAMD21_GCLK_GENCTRL_SRC_XOSC32K, AO_GCLK_XOSC32K);

	/*
	 * Use generator as source for dfm48m reference
	 */

	samd21_gclk_clkctrl(AO_GCLK_XOSC32K, SAMD21_GCLK_CLKCTRL_ID_DFLL48M_REF);
#endif

#ifdef AO_XOSC

	/* program DPLL */

	/* Divide down to 1MHz */
	samd21_sysctrl.dpllctrlb = (((AO_XOSC_DIV/2 - 1) << SAMD21_SYSCTRL_DPLLCTRLB_DIV) |
				    (0 << SAMD21_SYSCTRL_DPLLCTRLB_LBYPASS) |
				    (SAMD21_SYSCTRL_DPLLCTRLB_LTIME_DEFAULT << SAMD21_SYSCTRL_DPLLCTRLB_LTIME) |
				    (SAMD21_SYSCTRL_DPLLCTRLB_REFCLK_XOSC << SAMD21_SYSCTRL_DPLLCTRLB_REFCLK) |
				    (0 << SAMD21_SYSCTRL_DPLLCTRLB_WUF) |
				    (1 << SAMD21_SYSCTRL_DPLLCTRLB_LPEN) |
				    (SAMD21_SYSCTRL_DPLLCTRLB_FILTER_DEFAULT << SAMD21_SYSCTRL_DPLLCTRLB_FILTER));

	/* Multiply up to 48MHz */
	samd21_sysctrl.dpllratio = ((AO_XOSC_MUL - 1) << SAMD21_SYSCTRL_DPLLRATIO_LDR);

	/* Always on in run mode, off in standby mode */
	samd21_sysctrl.dpllctrla = ((0 << SAMD21_SYSCTRL_DPLLCTRLA_ONDEMAND) |
				    (0 << SAMD21_SYSCTRL_DPLLCTRLA_RUNSTDBY));

	/* Enable DPLL */
	samd21_sysctrl.dpllctrla |= (1 << SAMD21_SYSCTRL_DPLLCTRLA_ENABLE);

	/* Wait for the DPLL to be enabled */
	while ((samd21_sysctrl.dpllstatus & (1 << SAMD21_SYSCTRL_DPLLSTATUS_ENABLE)) == 0)
		;

	/* Wait for the DPLL to be ready */
	while ((samd21_sysctrl.dpllstatus & (1 << SAMD21_SYSCTRL_DPLLSTATUS_CLKRDY)) == 0)
		;

	/*
	 * Switch generator 0 (CPU clock) to DPLL
	 */

	/* divide by 1 */
	samd21_gclk_gendiv(AO_GCLK_SYSCLK, 1);

	/* select DPLL as source */
	samd21_gclk_genctrl(SAMD21_GCLK_GENCTRL_SRC_FDPLL96M, AO_GCLK_SYSCLK);
#else

	/*
	 * Enable DFLL48M clock
	 */

	samd21_sysctrl.dfllctrl = (1 << SAMD21_SYSCTRL_DFLLCTRL_ENABLE);
	samd21_dfll_wait_sync();

#ifdef AO_XOSC32K
	/* Set multiplier to get as close to 48MHz as we can without going over */
	samd21_sysctrl.dfllmul = (((31/4) << SAMD21_SYSCTRL_DFLLMUL_CSTEP) |
				  ((255/4) << SAMD21_SYSCTRL_DFLLMUL_FSTEP) |
				  ((AO_DFLL48M / AO_XOSC32K) << SAMD21_SYSCTRL_DFLLMUL_MUL));

	/* pull out coarse calibration value from rom */
	uint32_t coarse = ((samd21_aux1.calibration >> SAMD21_AUX1_CALIBRATION_DFLL48M_COARSE_CAL) &
			   SAMD21_AUX1_CALIBRATION_DFLL48M_COARSE_CAL_MASK);

	samd21_sysctrl.dfllval = ((coarse << SAMD21_SYSCTRL_DFLLVAL_COARSE) |
				  (512 << SAMD21_SYSCTRL_DFLLVAL_FINE));

	samd21_sysctrl.dfllctrl = 0;
	samd21_dfll_wait_sync();

	samd21_sysctrl.dfllctrl = ((1 << SAMD21_SYSCTRL_DFLLCTRL_MODE) |
				   (1 << SAMD21_SYSCTRL_DFLLCTRL_ENABLE));

	samd21_dfll_wait_sync();
	samd21_gclk_wait_sync();

	/* wait for fine lock */
	while ((samd21_sysctrl.pclksr & (1 << SAMD21_SYSCTRL_PCLKSR_DFLLLCKC)) == 0 ||
	       (samd21_sysctrl.pclksr & (1 << SAMD21_SYSCTRL_PCLKSR_DFLLLCKF)) == 0)
		;
#else
	samd21_sysctrl.dfllmul = (((31/4) << SAMD21_SYSCTRL_DFLLMUL_CSTEP) |
				  ((255/4) << SAMD21_SYSCTRL_DFLLMUL_FSTEP) |
				  ((AO_DFLL48M / 1000) << SAMD21_SYSCTRL_DFLLMUL_MUL));

	/* pull out coarse calibration value from rom */
	uint32_t coarse = ((samd21_aux1.calibration >> SAMD21_AUX1_CALIBRATION_DFLL48M_COARSE_CAL) &
			   SAMD21_AUX1_CALIBRATION_DFLL48M_COARSE_CAL_MASK);

	samd21_sysctrl.dfllval = ((coarse << SAMD21_SYSCTRL_DFLLVAL_COARSE) |
				  (512 << SAMD21_SYSCTRL_DFLLVAL_FINE));

	samd21_sysctrl.dfllctrl = 0;
	samd21_dfll_wait_sync();

	samd21_sysctrl.dfllctrl = ((1 << SAMD21_SYSCTRL_DFLLCTRL_MODE) |
				   (1 << SAMD21_SYSCTRL_DFLLCTRL_CCDIS) |
				   (1 << SAMD21_SYSCTRL_DFLLCTRL_USBCRM) |
				   (1 << SAMD21_SYSCTRL_DFLLCTRL_ENABLE));

	samd21_dfll_wait_sync();
	samd21_gclk_wait_sync();
#endif

	/*
	 * Switch generator 0 (CPU clock) to DFLL48M
	 */

	/* divide by 1 */
	samd21_gclk_gendiv(AO_GCLK_SYSCLK, 1);

	/* select DFLL48M as source */
	samd21_gclk_genctrl(SAMD21_GCLK_GENCTRL_SRC_DFLL48M, AO_GCLK_SYSCLK);
#endif

	/* Set up all of the clocks to be /1 */

	samd21_pm.cpusel = ((0 << SAMD21_PM_CPUSEL_CPUDIV));
	samd21_pm.apbasel = ((0 << SAMD21_PM_APBASEL_APBADIV));
	samd21_pm.apbbsel = ((0 << SAMD21_PM_APBBSEL_APBBDIV));
	samd21_pm.apbcsel = ((0 << SAMD21_PM_APBCSEL_APBCDIV));

	/* Disable OSC8M */
	samd21_sysctrl.osc8m &= ~(1 << SAMD21_SYSCTRL_OSC8M_ENABLE);

	/* Additional misc configuration stuff */

	/* Disable automatic NVM write operations */
	samd21_nvmctrl.ctrlb |= (1 << SAMD21_NVMCTRL_CTRLB_MANW);
}
