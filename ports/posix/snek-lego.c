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

static uint16_t	current_output;

static dacta_t	**dactas;
static size_t	num_dactas;

static int
snek_lego_init(char *lego)
{
	size_t    i;
	dacta_t	**new_dactas;
	dacta_t  *dacta;
	char	 name[16];

	for (i = 0; i < num_dactas; i++)
		if (!strcmp(lego, dactas[i]->name))
			return 1;

	new_dactas = malloc ((num_dactas + 1) * sizeof (dacta_t *));
	if (!new_dactas)
		return 0;

	dacta = calloc(1, sizeof(dacta_t) + strlen(lego) + 1);
	if (!dacta) {
		free(new_dactas);
		return 0;
	}
	strcpy(dacta->name, lego);

	for (i = 0; i < LEGO_OUTPUTS; i++) {
		dacta->outputs[i].power = 7;
		dacta->outputs[i].left = 0;
		dacta->outputs[i].on = 0;
		dacta->active[i].power = 0xff;
		dacta->active[i].left = 0xff;
		dacta->active[i].on = 0xff;


		if (num_dactas == 0)
			sprintf(name, "%c", 'A' + (int) i);
		else
			sprintf(name, "%c%d", 'A' + (int) i, (int) num_dactas);

		snek_id_t id;
		bool keyword;

		id = snek_name_id(name, &keyword);
		if (id != SNEK_ID_NONE) {
			snek_poly_t *ref = snek_global_ref(id, true);
			if (ref)
				*ref = snek_soffset_to_poly(num_dactas * LEGO_OUTPUTS + i + 1);
		}
	}

	current_output = 0;

	if (lego_start(dacta, lego) != LEGO_OK) {
		free(dacta);
		free(new_dactas);
		return 0;
	}
	memcpy(new_dactas, dactas, num_dactas * sizeof (dacta_t *));
	new_dactas[num_dactas++] = dacta;
	free(dactas);
	return 1;
}

static inline lego_sensor_t *
sensor(uint16_t pin)
{
	return &(dactas[pin / LEGO_SENSORS]->sensors[pin % LEGO_SENSORS]);
}

static inline lego_output_t *
output(uint16_t pin)
{
	return &(dactas[pin / LEGO_OUTPUTS]->outputs[pin % LEGO_OUTPUTS]);
}

static inline lego_output_t *
active(uint16_t pin)
{
	return &(dactas[pin / LEGO_OUTPUTS]->active[pin % LEGO_OUTPUTS]);
}

static inline bool
is_on(uint16_t pin)
{
	return output(pin)->on;
}

static inline bool
is_left(uint16_t pin)
{
	return output(pin)->left;
}

static inline void
set_left(uint16_t pin)
{
	output(pin)->left = 1;
}

static inline void
set_right(uint16_t pin)
{
	output(pin)->left = 0;
}

static inline void
set_on(uint16_t pin)
{
	output(pin)->on = 1;
}

static inline void
set_off(uint16_t pin)
{
	output(pin)->on = 0;
}

static inline void
set_power(uint16_t pin, uint8_t pow)
{
	output(pin)->power = pow;
}

static snek_poly_t
set_out(uint16_t pin)
{
	dacta_t		*dacta = dactas[pin / LEGO_OUTPUTS];

	pin = pin % LEGO_OUTPUTS;

	lego_output_t	*o = &dacta->outputs[pin];
	lego_output_t	*a = &dacta->active[pin];

	if (o->on != a->on) {
		if (!o->on) {
			lego_port_off(dacta, pin);
			a->on = o->on;
		}
	}
	if (o->left != a->left) {
		if (o->left)
			lego_set_left(dacta, pin);
		else
			lego_set_right(dacta, pin);
		a->left = o->left;
	}
	if (o->power != a->power) {
		lego_set_powers(dacta, o->power, 1 << pin);
		a->power = o->power;
	}
	if (o->on != a->on) {
		if (o->on) {
			lego_port_on(dacta, pin);
			a->on = o->on;
		}
	}
	return SNEK_NULL;
}

static uint16_t
snek_poly_get_output(snek_poly_t a)
{
	snek_offset_t p = snek_poly_get_soffset(a) - 1;
	if (num_dactas * LEGO_OUTPUTS <= p)
		snek_error_value(a);
	return p;
}

static uint16_t
snek_poly_get_input(snek_poly_t a)
{
	snek_offset_t p = snek_poly_get_soffset(a) - 1;
	if (num_dactas * LEGO_SENSORS <= p)
		snek_error_value(a);
	return p;
}

snek_poly_t
snek_builtin_talkto(snek_poly_t a)
{
	uint16_t	p = snek_poly_get_output(a);
	if (!snek_abort)
		current_output = p;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t a)
{
	float p = snek_poly_get_float(a);
	if (p < 0.0f) p = 0.0f;
	if (p > 1.0f) p = 1.0f;
	set_power(current_output, (uint8_t) (p * 7.0f + 0.5f));
	return set_out(current_output);
}

snek_poly_t
snek_builtin_setleft(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_left(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_setright(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_right(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_on(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_on(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_off(void)
{
	if (!dactas)
		return SNEK_NULL;
	set_off(current_output);
	return set_out(current_output);
}

snek_poly_t
snek_builtin_onfor(snek_poly_t a)
{
	if (!dactas)
		return SNEK_NULL;
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
	uint16_t p = snek_poly_get_input(a);
	if (snek_abort)
		return SNEK_NULL;

	dacta_t *dacta = dactas[p / LEGO_SENSORS];
	p = p % LEGO_SENSORS;
	lego_sensor_t	value = lego_sensor(dacta, p);
	return snek_float_to_poly((float) (1023 - value.value) / 1023.0f);
}

snek_poly_t
snek_builtin_stopall(void)
{
	uint16_t	p;
	for (p = 0; p < num_dactas * LEGO_OUTPUTS; p++) {
		set_off(p);
		set_out(p);
	}
	return SNEK_NULL;
}

/*
 * Return a string, or raise an error if the value isn't one
 */
static char *
snek_poly_get_string(snek_poly_t a)
{
	if (snek_poly_type(a) == snek_string)
		return snek_poly_to_string(a);
	snek_error_type_1(a);
	return NULL;
}

snek_poly_t
snek_builtin_lego_open(snek_poly_t a)
{
	char *name = snek_poly_get_string(a);
	if (!name)
		return SNEK_ZERO;
	if (snek_lego_init(name))
		return SNEK_ONE;
	return SNEK_ZERO;
}
