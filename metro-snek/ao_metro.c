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
#include <snek.h>
#include <ao_snek.h>
#include <ao_tcc_samd21.h>
#include <ao_tc_samd21.h>
#include <ao_adc_samd21.h>

void
ao_snek_set_pwm(void *gpio, uint8_t pin, void *timer, uint8_t config, uint16_t value)
{
	uint8_t	ch = config & AO_SNEK_TIMER_CH_MASK;
	uint8_t func = config & AO_SNEK_TIMER_FUNC_MASK;
	uint8_t dev = config & AO_SNEK_TIMER_DEV_MASK;

	if (dev == AO_SNEK_TIMER_DEV_TCC)
		ao_tcc_set(timer, ch&3, value);
	else
		ao_tc_set(timer, ch, value);
	switch (func) {
	case AO_SNEK_TIMER_FUNC_E:
		samd21_port_pmux_set(gpio, pin, SAMD21_PORT_PMUX_FUNC_E);
		break;
	case AO_SNEK_TIMER_FUNC_F:
		samd21_port_pmux_set(gpio, pin, SAMD21_PORT_PMUX_FUNC_F);
		break;
	}
}

void
ao_snek_clr_pwm(void *port, uint8_t pin)
{
	uint8_t pincfg = samd21_port_pincfg_get(port, pin);
	pincfg &= ~(1 << SAMD21_PORT_PINCFG_PMUXEN);
	samd21_port_pincfg_set(port, pin, pincfg);
}

uint16_t
ao_snek_get_adc(void *gpio, uint8_t pin, uint8_t adc)
{
	samd21_port_pmux_set(gpio, pin, SAMD21_PORT_PMUX_FUNC_B);
	return ao_adc_read(adc);
}

void
ao_snek_running(bool running)
{
	if (running)
		ao_led_off(AO_LED_TX);
	else
		ao_led_on(AO_LED_TX);
}

int _errno;

int *__errno(void)
{
	return &_errno;
}

int
main(void)
{
	ao_clock_init();

	ao_led_init();
	ao_led_on(AO_LED_TX);
	ao_timer_init();
	ao_tcc_samd21_init();
	ao_tc_samd21_init();
	ao_adc_init();
	ao_usb_init();

	ao_snek();

	return 0;
}
