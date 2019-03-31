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
#include <ao-snek.h>
#include <snek.h>

static uint8_t	power_pin;
static uint8_t	dir_pin;
static uint8_t	input_pin;
static uint16_t	power[AO_SNEK_NUM_PIN];
static uint32_t	on_pins;

struct ao_snek_pin {
	void	*gpio;
	uint8_t	pin;
	void	*timer;
	uint8_t	channel;
	uint8_t	adc;
};

static const struct ao_snek_pin ao_snek_pin[AO_SNEK_NUM_PIN] = {
	{ .gpio = AO_SNEK_GPIO_0, .pin = AO_SNEK_PIN_0, .timer = AO_SNEK_TIMER_0, .channel = AO_SNEK_CHANNEL_0, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_1, .pin = AO_SNEK_PIN_1, .timer = AO_SNEK_TIMER_1, .channel = AO_SNEK_CHANNEL_1, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_2, .pin = AO_SNEK_PIN_2, .timer = AO_SNEK_TIMER_2, .channel = AO_SNEK_CHANNEL_2, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_3, .pin = AO_SNEK_PIN_3, .timer = AO_SNEK_TIMER_3, .channel = AO_SNEK_CHANNEL_3, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_4, .pin = AO_SNEK_PIN_4, .timer = AO_SNEK_TIMER_4, .channel = AO_SNEK_CHANNEL_4, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_5, .pin = AO_SNEK_PIN_5, .timer = AO_SNEK_TIMER_5, .channel = AO_SNEK_CHANNEL_5, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_6, .pin = AO_SNEK_PIN_6, .timer = AO_SNEK_TIMER_6, .channel = AO_SNEK_CHANNEL_6, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_7, .pin = AO_SNEK_PIN_7, .timer = AO_SNEK_TIMER_7, .channel = AO_SNEK_CHANNEL_7, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_8, .pin = AO_SNEK_PIN_8, .timer = AO_SNEK_TIMER_8, .channel = AO_SNEK_CHANNEL_8, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_9, .pin = AO_SNEK_PIN_9, .timer = AO_SNEK_TIMER_9, .channel = AO_SNEK_CHANNEL_9, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_10, .pin = AO_SNEK_PIN_10, .timer = AO_SNEK_TIMER_10, .channel = AO_SNEK_CHANNEL_10, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_11, .pin = AO_SNEK_PIN_11, .timer = AO_SNEK_TIMER_11, .channel = AO_SNEK_CHANNEL_11, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_12, .pin = AO_SNEK_PIN_12, .timer = AO_SNEK_TIMER_12, .channel = AO_SNEK_CHANNEL_12, .adc = 0xff },
	{ .gpio = AO_SNEK_GPIO_13, .pin = AO_SNEK_PIN_13, .timer = AO_SNEK_TIMER_13, .channel = AO_SNEK_CHANNEL_13, .adc = 0xff },

	{ .gpio = AO_SNEK_GPIO_14, .pin = AO_SNEK_PIN_14, .timer = AO_SNEK_TIMER_14, .channel = AO_SNEK_CHANNEL_14, .adc = AO_SNEK_ADC_14 },
	{ .gpio = AO_SNEK_GPIO_15, .pin = AO_SNEK_PIN_15, .timer = AO_SNEK_TIMER_15, .channel = AO_SNEK_CHANNEL_15, .adc = AO_SNEK_ADC_15 },
	{ .gpio = AO_SNEK_GPIO_16, .pin = AO_SNEK_PIN_16, .timer = AO_SNEK_TIMER_16, .channel = AO_SNEK_CHANNEL_16, .adc = AO_SNEK_ADC_16 },
	{ .gpio = AO_SNEK_GPIO_17, .pin = AO_SNEK_PIN_17, .timer = AO_SNEK_TIMER_17, .channel = AO_SNEK_CHANNEL_17, .adc = AO_SNEK_ADC_17 },
	{ .gpio = AO_SNEK_GPIO_18, .pin = AO_SNEK_PIN_18, .timer = AO_SNEK_TIMER_18, .channel = AO_SNEK_CHANNEL_18, .adc = AO_SNEK_ADC_18 },
	{ .gpio = AO_SNEK_GPIO_19, .pin = AO_SNEK_PIN_19, .timer = AO_SNEK_TIMER_19, .channel = AO_SNEK_CHANNEL_19, .adc = AO_SNEK_ADC_19 },
};


void
ao_snek_port_init(void)
{
	uint8_t p;
	for (p = 0; p < AO_SNEK_NUM_PIN; p++) {
		ao_snek_clr_pwm(ao_snek_pin[p].gpio, ao_snek_pin[p].pin);
		ao_enable_input(ao_snek_pin[p].gpio, ao_snek_pin[p].pin, AO_EXTI_MODE_PULL_NONE);
	}

	memset(power, 0xff, sizeof(power));
}

static void
ao_snek_port_set_pwm(uint8_t p, uint16_t pwm)
{
	ao_snek_set_pwm(ao_snek_pin[0].gpio, ao_snek_pin[p].pin, ao_snek_pin[p].timer, ao_snek_pin[p].channel, pwm);
}

static void
ao_snek_port_clr_pwm(uint8_t p)
{
	ao_snek_clr_pwm(ao_snek_pin[0].gpio, ao_snek_pin[p].pin);
}

static void
ao_snek_port_set(uint8_t p, uint16_t value)
{
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
set_out(uint8_t pin)
{
	uint16_t	p = 0;

	if (is_on(pin))
		p = power[pin];

	if (has_pwm(pin)) {
		if (0 < p && p < SNEK_PWM_MAX) {
			ao_snek_port_set_pwm(pin, p);
			return SNEK_NULL;
		}
		ao_snek_port_clr_pwm(pin);
	}
	ao_snek_port_set(pin, p);
	return SNEK_NULL;
}

static void
set_dir(uint8_t pin, uint8_t d)
{
	if (d) {
		if (has_adc(pin))
			ao_snek_clr_adc(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin);
		ao_enable_output(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin, is_on(pin) && power[pin] != 0);
		set_out(pin);
	} else {
		if (has_adc(pin)) {
			ao_snek_set_adc(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin);
			ao_enable_input(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin, AO_EXTI_MODE_PULL_NONE);
		} else
			ao_enable_input(ao_snek_pin[pin].gpio, ao_snek_pin[pin].pin, AO_EXTI_MODE_PULL_UP);
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
snek_builtin_listento(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort) {
		set_dir(p, 0);
		input_pin = p;
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
	snek_builtin_off();
	return a;
}

#define analog_reference 1

snek_poly_t
snek_builtin_read(void)
{
	if (has_adc(input_pin)) {
		float value = ao_snek_port_get_analog(input_pin) / (float) AO_ADC_MAX;
		return snek_float_to_poly(value);
	} else {
		return snek_bool_to_poly(ao_snek_port_get(input_pin));
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
