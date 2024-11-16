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
#include <ao-snek.h>
#include <snek.h>

static uint8_t	power_pin;
static uint8_t	dir_pin;
static uint16_t	power[AO_SNEK_NUM_PIN];
static uint8_t	pull[AO_SNEK_NUM_PIN];

#ifdef AO_SNEK_PWM_RAMP_STEP
static int32_t	current_power[AO_SNEK_NUM_PIN];
static uint8_t	pwm_ramping;
#endif
static uint32_t	on_pins;

struct ao_snek_pin {
	void	*gpio;
	void	*timer;
	uint8_t	pin;
	uint8_t	channel;
	uint8_t	adc;
	uint8_t flags;
};

#ifndef AO_SNEK_FLAGS_0
#define AO_SNEK_FLAGS_0 0
#endif
#ifndef AO_SNEK_FLAGS_1
#define AO_SNEK_FLAGS_1 0
#endif
#ifndef AO_SNEK_FLAGS_2
#define AO_SNEK_FLAGS_2 0
#endif
#ifndef AO_SNEK_FLAGS_3
#define AO_SNEK_FLAGS_3 0
#endif
#ifndef AO_SNEK_FLAGS_4
#define AO_SNEK_FLAGS_4 0
#endif
#ifndef AO_SNEK_FLAGS_5
#define AO_SNEK_FLAGS_5 0
#endif
#ifndef AO_SNEK_FLAGS_6
#define AO_SNEK_FLAGS_6 0
#endif
#ifndef AO_SNEK_FLAGS_7
#define AO_SNEK_FLAGS_7 0
#endif
#ifndef AO_SNEK_FLAGS_8
#define AO_SNEK_FLAGS_8 0
#endif
#ifndef AO_SNEK_FLAGS_9
#define AO_SNEK_FLAGS_9 0
#endif
#ifndef AO_SNEK_FLAGS_10
#define AO_SNEK_FLAGS_10 0
#endif
#ifndef AO_SNEK_FLAGS_11
#define AO_SNEK_FLAGS_11 0
#endif
#ifndef AO_SNEK_FLAGS_12
#define AO_SNEK_FLAGS_12 0
#endif
#ifndef AO_SNEK_FLAGS_13
#define AO_SNEK_FLAGS_13 0
#endif
#ifndef AO_SNEK_FLAGS_14
#define AO_SNEK_FLAGS_14 0
#endif
#ifndef AO_SNEK_FLAGS_15
#define AO_SNEK_FLAGS_15 0
#endif
#ifndef AO_SNEK_FLAGS_16
#define AO_SNEK_FLAGS_16 0
#endif
#ifndef AO_SNEK_FLAGS_17
#define AO_SNEK_FLAGS_17 0
#endif
#ifndef AO_SNEK_FLAGS_18
#define AO_SNEK_FLAGS_18 0
#endif
#ifndef AO_SNEK_FLAGS_19
#define AO_SNEK_FLAGS_19 0
#endif
#ifndef AO_SNEK_FLAGS_20
#define AO_SNEK_FLAGS_20 0
#endif
#ifndef AO_SNEK_FLAGS_21
#define AO_SNEK_FLAGS_21 0
#endif
#ifndef AO_SNEK_FLAGS_22
#define AO_SNEK_FLAGS_22 0
#endif
#ifndef AO_SNEK_FLAGS_23
#define AO_SNEK_FLAGS_23 0
#endif
#ifndef AO_SNEK_FLAGS_24
#define AO_SNEK_FLAGS_24 0
#endif
#ifndef AO_SNEK_FLAGS_25
#define AO_SNEK_FLAGS_25 0
#endif
#ifndef AO_SNEK_FLAGS_26
#define AO_SNEK_FLAGS_26 0
#endif
#ifndef AO_SNEK_FLAGS_27
#define AO_SNEK_FLAGS_27 0
#endif
#ifndef AO_SNEK_FLAGS_28
#define AO_SNEK_FLAGS_28 0
#endif
#ifndef AO_SNEK_FLAGS_29
#define AO_SNEK_FLAGS_29 0
#endif
#ifndef AO_SNEK_FLAGS_30
#define AO_SNEK_FLAGS_30 0
#endif

static const struct ao_snek_pin ao_snek_pin[AO_SNEK_NUM_PIN] = {
	{ .gpio = AO_SNEK_GPIO_0, .pin = AO_SNEK_PIN_0, .timer = AO_SNEK_TIMER_0, .channel = AO_SNEK_CHANNEL_0, .adc = AO_SNEK_ADC_0, .flags = AO_SNEK_FLAGS_0 },
#if AO_SNEK_NUM_PIN > 1
	{ .gpio = AO_SNEK_GPIO_1, .pin = AO_SNEK_PIN_1, .timer = AO_SNEK_TIMER_1, .channel = AO_SNEK_CHANNEL_1, .adc = AO_SNEK_ADC_1, .flags = AO_SNEK_FLAGS_1 },
#endif
#if AO_SNEK_NUM_PIN > 2
	{ .gpio = AO_SNEK_GPIO_2, .pin = AO_SNEK_PIN_2, .timer = AO_SNEK_TIMER_2, .channel = AO_SNEK_CHANNEL_2, .adc = AO_SNEK_ADC_2, .flags = AO_SNEK_FLAGS_2 },
#endif
#if AO_SNEK_NUM_PIN > 3
	{ .gpio = AO_SNEK_GPIO_3, .pin = AO_SNEK_PIN_3, .timer = AO_SNEK_TIMER_3, .channel = AO_SNEK_CHANNEL_3, .adc = AO_SNEK_ADC_3, .flags = AO_SNEK_FLAGS_3 },
#endif
#if AO_SNEK_NUM_PIN > 4
	{ .gpio = AO_SNEK_GPIO_4, .pin = AO_SNEK_PIN_4, .timer = AO_SNEK_TIMER_4, .channel = AO_SNEK_CHANNEL_4, .adc = AO_SNEK_ADC_4, .flags = AO_SNEK_FLAGS_4 },
#endif
#if AO_SNEK_NUM_PIN > 5
	{ .gpio = AO_SNEK_GPIO_5, .pin = AO_SNEK_PIN_5, .timer = AO_SNEK_TIMER_5, .channel = AO_SNEK_CHANNEL_5, .adc = AO_SNEK_ADC_5, .flags = AO_SNEK_FLAGS_5 },
#endif
#if AO_SNEK_NUM_PIN > 6
	{ .gpio = AO_SNEK_GPIO_6, .pin = AO_SNEK_PIN_6, .timer = AO_SNEK_TIMER_6, .channel = AO_SNEK_CHANNEL_6, .adc = AO_SNEK_ADC_6, .flags = AO_SNEK_FLAGS_6 },
#endif
#if AO_SNEK_NUM_PIN > 7
	{ .gpio = AO_SNEK_GPIO_7, .pin = AO_SNEK_PIN_7, .timer = AO_SNEK_TIMER_7, .channel = AO_SNEK_CHANNEL_7, .adc = AO_SNEK_ADC_7, .flags = AO_SNEK_FLAGS_7 },
#endif
#if AO_SNEK_NUM_PIN > 8
	{ .gpio = AO_SNEK_GPIO_8, .pin = AO_SNEK_PIN_8, .timer = AO_SNEK_TIMER_8, .channel = AO_SNEK_CHANNEL_8, .adc = AO_SNEK_ADC_8, .flags = AO_SNEK_FLAGS_8 },
#endif
#if AO_SNEK_NUM_PIN > 9
	{ .gpio = AO_SNEK_GPIO_9, .pin = AO_SNEK_PIN_9, .timer = AO_SNEK_TIMER_9, .channel = AO_SNEK_CHANNEL_9, .adc = AO_SNEK_ADC_9, .flags = AO_SNEK_FLAGS_9 },
#endif
#if AO_SNEK_NUM_PIN > 10
	{ .gpio = AO_SNEK_GPIO_10, .pin = AO_SNEK_PIN_10, .timer = AO_SNEK_TIMER_10, .channel = AO_SNEK_CHANNEL_10, .adc = AO_SNEK_ADC_10, .flags = AO_SNEK_FLAGS_10 },
#endif
#if AO_SNEK_NUM_PIN > 11
	{ .gpio = AO_SNEK_GPIO_11, .pin = AO_SNEK_PIN_11, .timer = AO_SNEK_TIMER_11, .channel = AO_SNEK_CHANNEL_11, .adc = AO_SNEK_ADC_11, .flags = AO_SNEK_FLAGS_11 },
#endif
#if AO_SNEK_NUM_PIN > 12
	{ .gpio = AO_SNEK_GPIO_12, .pin = AO_SNEK_PIN_12, .timer = AO_SNEK_TIMER_12, .channel = AO_SNEK_CHANNEL_12, .adc = AO_SNEK_ADC_12, .flags = AO_SNEK_FLAGS_12 },
#endif
#if AO_SNEK_NUM_PIN > 13
	{ .gpio = AO_SNEK_GPIO_13, .pin = AO_SNEK_PIN_13, .timer = AO_SNEK_TIMER_13, .channel = AO_SNEK_CHANNEL_13, .adc = AO_SNEK_ADC_13, .flags = AO_SNEK_FLAGS_13 },
#endif
#if AO_SNEK_NUM_PIN > 14
	{ .gpio = AO_SNEK_GPIO_14, .pin = AO_SNEK_PIN_14, .timer = AO_SNEK_TIMER_14, .channel = AO_SNEK_CHANNEL_14, .adc = AO_SNEK_ADC_14, .flags = AO_SNEK_FLAGS_14 },
#endif
#if AO_SNEK_NUM_PIN > 15
	{ .gpio = AO_SNEK_GPIO_15, .pin = AO_SNEK_PIN_15, .timer = AO_SNEK_TIMER_15, .channel = AO_SNEK_CHANNEL_15, .adc = AO_SNEK_ADC_15, .flags = AO_SNEK_FLAGS_15 },
#endif
#if AO_SNEK_NUM_PIN > 16
	{ .gpio = AO_SNEK_GPIO_16, .pin = AO_SNEK_PIN_16, .timer = AO_SNEK_TIMER_16, .channel = AO_SNEK_CHANNEL_16, .adc = AO_SNEK_ADC_16, .flags = AO_SNEK_FLAGS_16 },
#endif
#if AO_SNEK_NUM_PIN > 17
	{ .gpio = AO_SNEK_GPIO_17, .pin = AO_SNEK_PIN_17, .timer = AO_SNEK_TIMER_17, .channel = AO_SNEK_CHANNEL_17, .adc = AO_SNEK_ADC_17, .flags = AO_SNEK_FLAGS_17 },
#endif
#if AO_SNEK_NUM_PIN > 18
	{ .gpio = AO_SNEK_GPIO_18, .pin = AO_SNEK_PIN_18, .timer = AO_SNEK_TIMER_18, .channel = AO_SNEK_CHANNEL_18, .adc = AO_SNEK_ADC_18, .flags = AO_SNEK_FLAGS_18 },
#endif
#if AO_SNEK_NUM_PIN > 19
	{ .gpio = AO_SNEK_GPIO_19, .pin = AO_SNEK_PIN_19, .timer = AO_SNEK_TIMER_19, .channel = AO_SNEK_CHANNEL_19, .adc = AO_SNEK_ADC_19, .flags = AO_SNEK_FLAGS_19 },
#endif
#if AO_SNEK_NUM_PIN > 20
	{ .gpio = AO_SNEK_GPIO_20, .pin = AO_SNEK_PIN_20, .timer = AO_SNEK_TIMER_20, .channel = AO_SNEK_CHANNEL_20, .adc = AO_SNEK_ADC_20, .flags = AO_SNEK_FLAGS_20 },
#endif
#if AO_SNEK_NUM_PIN > 21
	{ .gpio = AO_SNEK_GPIO_21, .pin = AO_SNEK_PIN_21, .timer = AO_SNEK_TIMER_21, .channel = AO_SNEK_CHANNEL_21, .adc = AO_SNEK_ADC_21, .flags = AO_SNEK_FLAGS_21 },
#endif
#if AO_SNEK_NUM_PIN > 22
	{ .gpio = AO_SNEK_GPIO_22, .pin = AO_SNEK_PIN_22, .timer = AO_SNEK_TIMER_22, .channel = AO_SNEK_CHANNEL_22, .adc = AO_SNEK_ADC_22, .flags = AO_SNEK_FLAGS_22 },
#endif
#if AO_SNEK_NUM_PIN > 23
	{ .gpio = AO_SNEK_GPIO_23, .pin = AO_SNEK_PIN_23, .timer = AO_SNEK_TIMER_23, .channel = AO_SNEK_CHANNEL_23, .adc = AO_SNEK_ADC_23, .flags = AO_SNEK_FLAGS_23 },
#endif
#if AO_SNEK_NUM_PIN > 24
	{ .gpio = AO_SNEK_GPIO_24, .pin = AO_SNEK_PIN_24, .timer = AO_SNEK_TIMER_24, .channel = AO_SNEK_CHANNEL_24, .adc = AO_SNEK_ADC_24, .flags = AO_SNEK_FLAGS_24 },
#endif
#if AO_SNEK_NUM_PIN > 25
	{ .gpio = AO_SNEK_GPIO_25, .pin = AO_SNEK_PIN_25, .timer = AO_SNEK_TIMER_25, .channel = AO_SNEK_CHANNEL_25, .adc = AO_SNEK_ADC_25, .flags = AO_SNEK_FLAGS_25 },
#endif
#if AO_SNEK_NUM_PIN > 26
	{ .gpio = AO_SNEK_GPIO_26, .pin = AO_SNEK_PIN_26, .timer = AO_SNEK_TIMER_26, .channel = AO_SNEK_CHANNEL_26, .adc = AO_SNEK_ADC_26, .flags = AO_SNEK_FLAGS_26 },
#endif
#if AO_SNEK_NUM_PIN > 27
	{ .gpio = AO_SNEK_GPIO_27, .pin = AO_SNEK_PIN_27, .timer = AO_SNEK_TIMER_27, .channel = AO_SNEK_CHANNEL_27, .adc = AO_SNEK_ADC_27, .flags = AO_SNEK_FLAGS_27 },
#endif
#if AO_SNEK_NUM_PIN > 28
	{ .gpio = AO_SNEK_GPIO_28, .pin = AO_SNEK_PIN_28, .timer = AO_SNEK_TIMER_28, .channel = AO_SNEK_CHANNEL_28, .adc = AO_SNEK_ADC_28, .flags = AO_SNEK_FLAGS_28 },
#endif
#if AO_SNEK_NUM_PIN > 29
	{ .gpio = AO_SNEK_GPIO_29, .pin = AO_SNEK_PIN_29, .timer = AO_SNEK_TIMER_29, .channel = AO_SNEK_CHANNEL_29, .adc = AO_SNEK_ADC_29, .flags = AO_SNEK_FLAGS_29 },
#endif
#if AO_SNEK_NUM_PIN > 30
	{ .gpio = AO_SNEK_GPIO_30, .pin = AO_SNEK_PIN_30, .timer = AO_SNEK_TIMER_30, .channel = AO_SNEK_CHANNEL_30, .adc = AO_SNEK_ADC_30, .flags = AO_SNEK_FLAGS_30 },
#endif
#if AO_SNEK_NUM_PIN > 31
#error too many pins
#endif
};


#ifdef NEOPIXEL_RESET
static void
set_dir(uint8_t pin, uint8_t d);

static const struct snek_neopixel reset_neopixels[NEOPIXEL_RESET];

#ifndef NEOPIXEL_RESET_DIR_PIN
#define NEOPIXEL_RESET_DIR_PIN NEOPIXEL_RESET_POWER_PIN
#endif

static void
neopixel_reset_delay(void)
{
	uint32_t	i;
	for (i = 0; i < 1000; i++)
		ao_arch_nop();
}
#endif

void
ao_snek_port_init(void)
{
	uint8_t p;
	for (p = 0; p < AO_SNEK_NUM_PIN; p++) {
		ao_snek_clr_pwm(ao_snek_pin[p].gpio, ao_snek_pin[p].pin);
		ao_enable_input(ao_snek_pin[p].gpio, ao_snek_pin[p].pin, AO_MODE_PULL_NONE);
		pull[p] = AO_MODE_PULL_NONE;
		if (ao_snek_pin[p].flags & SNEK_PIN_PULL_DOWN)
			pull[p] = AO_MODE_PULL_DOWN;
		else if (ao_snek_pin[p].flags & SNEK_PIN_PULL_UP)
			pull[p] = AO_MODE_PULL_UP;
	}

	memset(power, 0xff, sizeof(power));
#ifdef NEOPIXEL_RESET
#if NEOPIXEL_RESET_POWER_PIN == NEOPIXEL_RESET_DIR_PIN
	set_dir(NEOPIXEL_RESET_POWER_PIN, 1);
	neopixel_reset_delay();
	ao_snek_neopixel_write(ao_snek_pin[NEOPIXEL_RESET_POWER_PIN].gpio,
			       ao_snek_pin[NEOPIXEL_RESET_POWER_PIN].pin,
			       NEOPIXEL_RESET, reset_neopixels);
	neopixel_reset_delay();
	set_dir(NEOPIXEL_RESET_POWER_PIN, 0);
#else
	set_dir(NEOPIXEL_RESET_POWER_PIN, 1);
	set_dir(NEOPIXEL_RESET_DIR_PIN, 1);
	neopixel_reset_delay();
	ao_snek_apa102_write(ao_snek_pin[NEOPIXEL_RESET_POWER_PIN].gpio,
			     ao_snek_pin[NEOPIXEL_RESET_POWER_PIN].pin,
			     ao_snek_pin[NEOPIXEL_RESET_DIR_PIN].gpio,
			     ao_snek_pin[NEOPIXEL_RESET_DIR_PIN].pin,
			     NEOPIXEL_RESET, reset_neopixels);
	neopixel_reset_delay();
	set_dir(NEOPIXEL_RESET_POWER_PIN, 0);
	set_dir(NEOPIXEL_RESET_DIR_PIN, 0);
#endif
#endif
}

static bool
invert_pin(uint8_t p)
{
	return ao_snek_pin[p].flags & SNEK_PIN_INVERT;
}

static void
ao_snek_port_set_pwm(uint8_t p, uint16_t pwm)
{
	if (invert_pin(p))
		pwm = SNEK_PWM_MAX - pwm;
	ao_snek_set_pwm(ao_snek_pin[p].gpio, ao_snek_pin[p].pin, ao_snek_pin[p].timer, ao_snek_pin[p].channel, pwm);
}

static void
ao_snek_port_clr_pwm(uint8_t p)
{
	ao_snek_clr_pwm(ao_snek_pin[p].gpio, ao_snek_pin[p].pin);
}

static void
ao_snek_port_set_dac(uint8_t p, uint16_t value)
{
	ao_snek_set_dac(ao_snek_pin[p].gpio, ao_snek_pin[p].pin, value);
}

static void
ao_snek_port_clr_dac(uint8_t p)
{
	ao_snek_clr_dac(ao_snek_pin[p].gpio, ao_snek_pin[p].pin);
}

static void
ao_snek_port_set(uint8_t p, uint16_t value)
{
	if (invert_pin(p))
		value = !value;
	ao_gpio_set(ao_snek_pin[p].gpio, ao_snek_pin[p].pin, !!value);
}

static uint8_t
ao_snek_port_get(uint8_t p)
{
	return ao_gpio_get(ao_snek_pin[p].gpio, ao_snek_pin[p].pin);
}

static uint16_t
ao_snek_port_get_analog(uint8_t p)
{
	return ao_snek_get_adc(ao_snek_pin[p].adc);
}

static bool
has_pwm(uint8_t p)
{
	return ao_snek_pin[p].timer != NULL;
}

static bool
has_dac(uint8_t p)
{
	return ao_snek_pin[p].gpio == &samd21_port_a && ao_snek_pin[p].pin == 2;
}

#ifdef AO_SNEK_PWM_RAMP_STEP
static bool
has_ramp_pwm(uint8_t p)
{
	return (ao_snek_pin[p].flags & SNEK_PIN_RAMP_PWM) != 0;
}

static bool
has_ramp_dir(uint8_t p)
{
	return p > 0 && (ao_snek_pin[p-1].flags & SNEK_PIN_RAMP_PWM) != 0;
}
#endif

static bool
has_adc(uint8_t p)
{
	return ao_snek_pin[p].adc != 0xff;
}

static bool
is_on(uint8_t pin)
{
	return (on_pins >> pin) & 1;
}

static void
set_on(uint8_t pin)
{
	on_pins |= ((uint32_t) 1) << pin;
}

static void
set_off(uint8_t pin)
{
	on_pins &= ~(((uint32_t) 1) << pin);
}

static snek_poly_t
_set_out(uint8_t pin, uint16_t p)
{
#ifdef AO_SNEK_PWM_RAMP_STEP
	if (has_ramp_dir(pin) || has_ramp_pwm(pin)) {
		pwm_ramping = 1;
		return SNEK_NULL;
	}
#endif
	if (has_dac(pin)) {
		ao_snek_port_set_dac(pin, p);
		return SNEK_NULL;
	}
	if (has_pwm(pin)) {
		if ((0 < p && p < SNEK_PWM_MAX)) {
			ao_snek_port_set_pwm(pin, p);
			return SNEK_NULL;
		}
		ao_snek_port_clr_pwm(pin);
	}
	ao_snek_port_set(pin, p);
	return SNEK_NULL;
}

static void
set_in(uint8_t pin)
{
	if (has_pwm(pin))
		ao_snek_port_clr_pwm(pin);
	else if (has_dac(pin))
		ao_snek_port_clr_dac(pin);
}

#ifdef SNEK_DRV8833
static snek_poly_t
set_out(uint8_t pin)
{
	uint16_t	p = 0;

	if (IS_DRV8833_DIR(pin) || IS_DRV8833_PWM(pin)) {
		uint8_t	dir, pwm;

		if (IS_DRV8833_DIR(pin)) {
			dir = pin;
			pwm = DRV8833_FIND_PWM(pin);
		} else {
			pwm = pin;
			dir = DRV8833_FIND_DIR(pin);
		}
		if (is_on(pwm))
			p = power[pwm];
		if (is_on(dir)) {
			_set_out(pwm, 0);
			return _set_out(dir, p);
		} else {
			_set_out(dir, 0);
			return _set_out(pwm, p);
		}
	} else {
		if (is_on(pin))
			p = power[pin];

		return _set_out(pin, p);
	}
}
#else
static snek_poly_t set_out(uint8_t pin)
{
	uint16_t	p = 0;

	if (is_on(pin))
		p = power[pin];

	return _set_out(pin, p);
}
#endif

#ifdef AO_SNEK_PWM_RAMP_STEP
static int32_t
ao_snek_pwm_power(uint8_t p)
{
	if (!is_on(p))
		return 0;
	if (is_on(p+1))
		return (int32_t) power[p];
	else
		return - (int32_t) power[p];
}

void
ao_snek_step_pwm(void)
{
	uint8_t p;
	if (!pwm_ramping)
		return;

	pwm_ramping = 0;
	for (p = 0; p < AO_SNEK_NUM_PIN; p++) {
		if (ao_snek_pin[p].flags & SNEK_PIN_RAMP_PWM) {
			int32_t pow = ao_snek_pwm_power(p);
			if (current_power[p] != pow) {

				/* Compute next power value */
				int32_t change = pow - current_power[p];

				if (change < -AO_SNEK_PWM_RAMP_STEP)
					change = -AO_SNEK_PWM_RAMP_STEP;
				else if (change > AO_SNEK_PWM_RAMP_STEP)
					change = AO_SNEK_PWM_RAMP_STEP;
				current_power[p] += change;

				/* Convert power value into dir/pwm */
				bool dval;
				uint16_t pval;
				if (current_power[p] >= 0) {
					dval = true;
					pval = (uint16_t) current_power[p];
				} else {
					dval = false;
					pval = (uint16_t) -current_power[p];
				}

				/* Set dir and pwm pins */
				ao_snek_port_set(p + 1, dval);
				ao_snek_port_set_pwm(p, pval);

				/* Note that we're still changing the value */
				pwm_ramping = 1;
			}
		}
	}
}
#endif

static void
set_dir(uint8_t pin, uint8_t d)
{
	if (d) {
		if (has_adc(pin))
			ao_snek_clr_adc(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin);
		ao_enable_output(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin, is_on(pin) && power[pin] != 0);
		set_out(pin);
	} else {
		set_in(pin);
		uint32_t mode = AO_MODE_PULL_UP;
		switch (pull[pin]) {
		case AO_MODE_PULL_NONE:
			mode = AO_MODE_PULL_NONE;
			break;
		case AO_MODE_PULL_DOWN:
			mode = AO_MODE_PULL_DOWN;
			break;
		case AO_MODE_PULL_UP:
			mode = AO_MODE_PULL_UP;
			break;
		}
		if (has_adc(pin)) {
			if (mode == AO_MODE_PULL_NONE)
				ao_snek_set_adc(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin);
			else
				ao_snek_clr_adc(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin);
		}
		ao_enable_input(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin, mode);
	}
}

static uint8_t
snek_poly_get_pin(snek_poly_t a)
{
	snek_soffset_t p = snek_poly_get_soffset(a);
	if (p < 0 || AO_SNEK_NUM_PIN <= p)
		snek_error_value(a);
	return p;
}

snek_poly_t
snek_builtin_talkto(snek_poly_t a)
{
	snek_list_t *l;
	uint8_t p, d;

	if (snek_poly_type(a) == snek_list) {
		l = snek_poly_to_list(a);
		p = snek_poly_get_pin(snek_list_get(l, SNEK_ZERO, true));
		d = snek_poly_get_pin(snek_list_get(l, SNEK_ONE, true));
	} else {
		p = d = snek_poly_get_pin(a);
	}
	if (!snek_abort) {
		set_dir(p, 1);
		if (p != d)
			set_dir(d, 1);
		power_pin = p;
		dir_pin = d;
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t a)
{
	float p = snek_poly_get_float(a);
	if (p < 0.0f) p = 0.0f;
	if (p > 1.0f) p = 1.0f;
	power[power_pin] = (uint16_t) (p * SNEK_PWM_MAX + 0.5f);
	return set_out(power_pin);
}

snek_poly_t
snek_builtin_setleft(void)
{
	set_on(dir_pin);
	return set_out(dir_pin);
}

snek_poly_t
snek_builtin_setright(void)
{
	set_off(dir_pin);
	return set_out(dir_pin);
}

snek_poly_t
snek_builtin_on(void)
{
	set_on(power_pin);
	return set_out(power_pin);
}

snek_poly_t
snek_builtin_off(void)
{
	set_off(power_pin);
	return set_out(power_pin);
}

snek_poly_t
snek_builtin_onfor(snek_poly_t a)
{
	snek_builtin_on();
	snek_builtin_time_sleep(a);
	return snek_builtin_off();
}

snek_poly_t
snek_builtin_pullnone(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort)
		pull[p] = AO_MODE_PULL_NONE;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pullup(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort)
		pull[p] = AO_MODE_PULL_UP;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pulldown(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort)
		pull[p] = AO_MODE_PULL_DOWN;
	return SNEK_NULL;
}

#define analog_reference 1

snek_poly_t
snek_builtin_read(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (snek_abort)
		return SNEK_NULL;
	set_dir(p, 0);

	if (has_adc(p) && pull[p] == AO_MODE_PULL_NONE) {
		float value = ao_snek_port_get_analog(p) / (float) AO_ADC_MAX;
		return snek_float_to_poly(value);
	} else {
		return snek_bool_to_poly(ao_snek_port_get(p));
	}
}

snek_poly_t
snek_builtin_stopall(void)
{
	uint8_t p;
	for (p = 0; p < AO_SNEK_NUM_PIN; p++)
		if (on_pins & ((uint32_t) 1 << p)) {
			set_off(p);
			set_out(p);
		}
	return SNEK_NULL;
}


static uint8_t
snek_poly_to_inten(snek_poly_t a)
{
	float f = snek_poly_get_float(a);
	if (f < 0.0f) f = 0.0f;
	if (f > 1.0f) f = 1.0f;
	return (uint8_t) (f * 255.0f + 0.5f);
}

struct snek_neopixel *snek_neopixels;
static snek_offset_t	snek_neopixel_count;

static snek_poly_t
set_neopixel(int p, snek_poly_t pixel)
{
	if (snek_poly_type(pixel) != snek_list)
		return snek_error_type_1(pixel);
	snek_list_t *pixel_list = snek_poly_to_list(pixel);
	if (snek_list_type(pixel_list) == snek_list_dict || pixel_list->size != 3)
		return snek_error_type_1(pixel);
	snek_poly_t *pixel_data = snek_list_data(pixel_list);
	snek_neopixels[p].r = snek_poly_to_inten(pixel_data[0]);
	snek_neopixels[p].g = snek_poly_to_inten(pixel_data[1]);
	snek_neopixels[p].b = snek_poly_to_inten(pixel_data[2]);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_neopixel(snek_poly_t pixels)
{
	if (snek_poly_type(pixels) != snek_list)
		return snek_error_type_1(pixels);
	snek_list_t *pixels_list = snek_poly_to_list(pixels);
	if (snek_list_type(pixels_list) == snek_list_dict)
		return snek_error_type_1(pixels);

	snek_poly_t *pixels_data = snek_list_data(pixels_list);
	snek_offset_t list_size = pixels_list->size;

	/* Did the user pass a list of three elements, the first of which is a number? */
	bool is_immediate = (list_size == 3 && snek_poly_type(pixels_data[0]) == snek_float);

	if (is_immediate)
		list_size = 1;

	if (snek_neopixels == NULL || snek_neopixel_count < list_size) {
		snek_stack_push_list(pixels_list);
		snek_neopixels = snek_alloc(list_size * sizeof (struct snek_neopixel));
		pixels_list = snek_stack_pop_list();
		pixels_data = snek_list_data(pixels_list);
		if (!snek_neopixels)
			return SNEK_NULL;
		snek_neopixel_count = list_size;
	}

	if (is_immediate)
		set_neopixel(0, pixels);
	else {
		for (int p = 0; p < pixels_list->size; p++) {
			set_neopixel(p, pixels_data[p]);
			if (snek_abort)
				return SNEK_NULL;
		}
	}
	if (power_pin == dir_pin)
		ao_snek_neopixel_write(ao_snek_pin[power_pin].gpio, ao_snek_pin[power_pin].pin,
				       pixels_list->size, snek_neopixels);
	else
		ao_snek_apa102_write(ao_snek_pin[power_pin].gpio,
				     ao_snek_pin[power_pin].pin,
				     ao_snek_pin[dir_pin].gpio,
				     ao_snek_pin[dir_pin].pin,
				     pixels_list->size, snek_neopixels);
	return SNEK_NULL;
}

#ifdef SNEK_SAMD21_DAC_TIMER
#include <ao-dac-samd21.h>

snek_poly_t
snek_builtin_tone(snek_poly_t a)
{
	float f = snek_poly_get_float(a);
	if (!snek_abort)
		ao_dac_set_hz(f);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_tonefor(snek_poly_t a, snek_poly_t b)
{
	snek_builtin_tone(a);
	snek_builtin_on();
	snek_builtin_time_sleep(b);
	return snek_builtin_off();
}
#endif
