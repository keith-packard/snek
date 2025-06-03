/*
 * Copyright © 2025 Keith Packard <keithp@keithp.com>
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

#include "snek.h"
#include "snek-70909.h"

static lego_buf_t      	lego_buf;

static uint8_t	output;

typedef struct {
	uint8_t	power;
	uint8_t left;
	uint8_t on;
} lego_output_t;

static lego_output_t	outputs[LEGO_OUTPUTS];
static lego_output_t	active[LEGO_OUTPUTS];

int snek_lego_init(char *lego)
{
	int i;
	for (i = 0; i < LEGO_OUTPUTS; i++) {
		outputs[i].power = 7;
		outputs[i].left = 0;
		outputs[i].on = 0;
		active[i].power = 0xff;
		active[i].left = 0xff;
		active[i].on = 0xff;
	}
	output = 0;

	if (lego_start(&lego_buf, lego) != LEGO_OK)
		return 0;
	return 1;
}

static inline bool
is_on(uint8_t pin)
{
	return outputs[pin].on;
}

static inline bool
is_left(uint8_t pin)
{
	return outputs[pin].left;
}

static inline void
set_left(uint8_t pin)
{
	outputs[pin].left = 1;
}

static inline void
set_right(uint8_t pin)
{
	outputs[pin].left = 0;
}

static inline void
set_on(uint8_t pin)
{
	outputs[pin].on = 1;
}

static inline void
set_off(uint8_t pin)
{
	outputs[pin].on = 0;
}

static inline void
set_power(uint8_t pin, uint8_t pow)
{
	outputs[pin].power = pow;
}

static snek_poly_t
set_out(uint8_t pin)
{
	lego_output_t	*o = &outputs[pin];
	lego_output_t	*a = &active[pin];

	if (o->on != a->on) {
		if (!o->on) {
			lego_port_off(&lego_buf, pin);
			a->on = o->on;
		}
	}
	if (o->left != a->left) {
		if (o->left)
			lego_set_left(&lego_buf, pin);
		else
			lego_set_right(&lego_buf, pin);
		a->left = o->left;
	}
	if (o->power != a->power) {
		lego_set_powers(&lego_buf, o->power, 1 << pin);
		a->power = o->power;
	}
	if (o->on != a->on) {
		if (o->on) {
			lego_port_on(&lego_buf, pin);
			a->on = o->on;
		}
	}
	return SNEK_NULL;
}

static uint8_t
snek_poly_get_output(snek_poly_t a)
{
	snek_soffset_t p = snek_poly_get_soffset(a) - 1;
	if (p < 0 || LEGO_OUTPUTS <= p)
		snek_error_value(a);
	return p;
}

static uint8_t
snek_poly_get_input(snek_poly_t a)
{
	snek_soffset_t p = snek_poly_get_soffset(a) - 1;
	if (p < 0 || LEGO_SENSORS <= p)
		snek_error_value(a);
	return p;
}

snek_poly_t
snek_builtin_talkto(snek_poly_t a)
{
	uint8_t	p = snek_poly_get_output(a);
	if (!snek_abort)
		output = p;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t a)
{
	float p = snek_poly_get_float(a);
	if (p < 0.0f) p = 0.0f;
	if (p > 1.0f) p = 1.0f;
	set_power(output, (uint8_t) (p * 7.0f + 0.5f));
	return set_out(output);
}

snek_poly_t
snek_builtin_setleft(void)
{
	set_left(output);
	return set_out(output);
}

snek_poly_t
snek_builtin_setright(void)
{
	set_right(output);
	return set_out(output);
}

snek_poly_t
snek_builtin_on(void)
{
	set_on(output);
	return set_out(output);
}

snek_poly_t
snek_builtin_off(void)
{
	set_off(output);
	return set_out(output);
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
	(void) a;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pullup(snek_poly_t a)
{
	(void) a;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_read(snek_poly_t a)
{
	uint8_t p = snek_poly_get_input(a);
	if (snek_abort)
		return SNEK_NULL;
	lego_sensor_t	value = lego_sensor(&lego_buf, p);
	return snek_float_to_poly((float) (1023 - value.value) / 1023.0f);
}

snek_poly_t
snek_builtin_stopall(void)
{
	uint8_t	p;
	for (p = 0; p < LEGO_OUTPUTS; p++) {
		set_off(p);
		set_out(p);
	}
	return SNEK_NULL;
}
