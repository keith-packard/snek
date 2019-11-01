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
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <snek.h>
#include <metal/gpio.h>
#include <metal/machine.h>
#include <metal/uart.h>
#include <metal/init.h>

#define NUM_PIN 3

static uint8_t pin_map[NUM_PIN] = {
	22,
	19,
	21
};

static uint8_t	power_pin, dir_pin;
static uint8_t	power[NUM_PIN];
static uint8_t	on_pins[NUM_PIN];

static bool
is_on(uint8_t pin)
{
	return on_pins[pin];
}

static void
set_on(uint8_t pin)
{
	on_pins[pin] = true;
}

static void
set_off(uint8_t pin)
{
	on_pins[pin] = false;
}

static void
set_dir(uint8_t p, uint8_t d)
{
	struct metal_gpio *gpio = metal_gpio_get_device(0);
	uint8_t pin = pin_map[p];

	if (d) {
		metal_gpio_disable_input(gpio, pin);
		metal_gpio_enable_output(gpio, pin);
	} else {
		metal_gpio_disable_output(gpio, pin);
		metal_gpio_enable_input(gpio, pin);
	}
}

static snek_poly_t
set_out(uint8_t p)
{
	struct metal_gpio *gpio = metal_gpio_get_device(0);
	uint8_t pow = 0;

	if (is_on(p))
		pow = power[p];

	uint8_t pin = pin_map[p];

	metal_gpio_set_pin(gpio, pin, !pow);
	return SNEK_NULL;
}

static uint8_t
snek_poly_get_pin(snek_poly_t a)
{
	snek_soffset_t p = snek_poly_get_soffset(a);
	if (p < 0 || NUM_PIN <= p)
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
		set_out(p);
		set_dir(p, 1);
		set_out(d);
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
	power[power_pin] = (uint8_t) (p * 255.0f + 0.5f);
	return set_out(power_pin);
}

snek_poly_t
snek_builtin_setleft(void)
{
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setright(void)
{
	return SNEK_NULL;
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
snek_builtin_pullnone(snek_poly_t a)
{
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pullup(snek_poly_t a)
{
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pulldown(snek_poly_t a)
{
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_read(snek_poly_t a)
{
	return SNEK_ZERO;
}

snek_poly_t
snek_builtin_onfor(snek_poly_t a)
{
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_stopall(void)
{
	return SNEK_NULL;
}

METAL_CONSTRUCTOR(snek_gpio_init) {
	memset(power, 0xff, NUM_PIN);
}
