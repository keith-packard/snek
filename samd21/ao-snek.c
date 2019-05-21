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
#include <snek.h>
#include <ao-snek.h>
#include <ao-tcc-samd21.h>
#include <ao-tc-samd21.h>
#include <ao-adc-samd21.h>
#include <setjmp.h>

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
	samd21_port_pincfg_set(gpio, pin,
			       (1 << SAMD21_PORT_PINCFG_DRVSTR) |
			       (1 << SAMD21_PORT_PINCFG_PULLEN) |
			       (1 << SAMD21_PORT_PINCFG_INEN),
			       (0 << SAMD21_PORT_PINCFG_DRVSTR) |
			       (0 << SAMD21_PORT_PINCFG_PULLEN) |
			       (0 << SAMD21_PORT_PINCFG_INEN));
}

static void
ao_snek_clr_pmux(void *port, uint8_t pin)
{
	samd21_port_pincfg_set(port, pin, (1 << SAMD21_PORT_PINCFG_PMUXEN), (0 << SAMD21_PORT_PINCFG_PMUXEN));
}

void
ao_snek_clr_pwm(void *port, uint8_t pin)
{
	ao_snek_clr_pmux(port, pin);
}

void
ao_snek_set_adc(void *gpio, uint8_t pin)
{
	samd21_port_pmux_set(gpio, pin, SAMD21_PORT_PMUX_FUNC_B);
	samd21_port_pincfg_set(gpio, pin,
			       (1 << SAMD21_PORT_PINCFG_DRVSTR) |
			       (1 << SAMD21_PORT_PINCFG_PULLEN) |
			       (1 << SAMD21_PORT_PINCFG_INEN),
			       (0 << SAMD21_PORT_PINCFG_DRVSTR) |
			       (0 << SAMD21_PORT_PINCFG_PULLEN) |
			       (1 << SAMD21_PORT_PINCFG_INEN));
}

void
ao_snek_clr_adc(void *gpio, uint8_t pin)
{
	ao_snek_clr_pmux(gpio, pin);
}

uint16_t
ao_snek_get_adc(uint8_t adc)
{
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

void
ao_usb_out_hook(uint8_t *hook_buf, uint16_t len)
{
	uint16_t i;

	for (i = 0; i < len; i++)
		if (hook_buf[i] == ('c' & 0x1f))
			snek_abort = true;
}

int _errno;

int *__errno(void)
{
	return &_errno;
}

static jmp_buf	snek_reset_buf;

extern char __snek_data_start__, __snek_data_end__;
extern char __snek_bss_start__, __snek_bss_end__;
extern char __text_start__, __text_end__;
extern char __data_start__, __data_end__;
extern char __bss_start__, __bss_end__;

snek_poly_t
snek_builtin_reset(void)
{
	/* reset data */
	memcpy(&__snek_data_start__,
	       &__text_end__ + (&__snek_data_start__ - &__data_start__),
	       &__snek_data_end__ - &__snek_data_start__);

	/* reset bss */
	memset(&__snek_bss_start__, '\0', &__snek_bss_end__ - &__snek_bss_start__);

	/* and off we go! */
	longjmp(snek_reset_buf, 1);
	return SNEK_NULL;
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

	setjmp(snek_reset_buf);
	ao_snek_port_init();
	snek_init();
	snek_builtin_eeprom_load();
	printf("Welcome to snek " SNEK_VERSION "\n");
	fflush(stdout);
	ao_snek_running(false);
	for (;;) {
		snek_interactive = true;
		snek_parse();
	}
}
