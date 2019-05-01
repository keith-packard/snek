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

static uint32_t
snek_millis(void)
{
	return ao_time_ns() / 1000000;
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	uint32_t	expire = snek_millis() + (snek_poly_get_float(a) * 1000.0f + 0.5f);
	while (!snek_abort && (int32_t) (expire - snek_millis()) > 0)
		;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly(snek_millis() / 1000.0f);
}

static uint64_t random_x, random_w;

#define random_s 0xb5ad4eceda1ce2a9ULL

snek_poly_t
snek_builtin_random_seed(snek_poly_t a)
{
	random_x = a.u;
	random_x |= random_x << 32;
	random_w = 0;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	random_x *= random_x;
	random_w += random_s;
	random_x += random_w;
	random_x = (random_x >> 32) | (random_x << 32);
	return snek_float_to_poly(random_x % snek_poly_get_soffset(a));
}

#if HAS_I2C
#include <ao-i2c.h>

snek_poly_t
snek_builtin_i2c_recv(snek_poly_t addr, snek_poly_t data)
{
	snek_offset_t	a = (snek_poly_get_soffset(addr) << 1) | 1;
	snek_list_t	*list;
	uint8_t		tmp[32];
	uint8_t		i;

	if (snek_poly_type(data) != snek_list ||
	    snek_list_type(list = snek_poly_to_list(data)) != snek_list_list ||
	    list->size > sizeof (data))
	{
		return snek_error_type_1(data);
	}
	if (snek_abort)
		return SNEK_NULL;
	ao_i2c_start(a);
	ao_i2c_recv(tmp, list->size, true);
	snek_poly_t	*d = snek_list_data(list);
	for (i = 0; i < list->size; i++)
		d[i] = snek_float_to_poly(tmp[i]);
	return snek_float_to_poly(list->size);
}

snek_poly_t
snek_builtin_i2c_send(snek_poly_t addr, snek_poly_t data)
{
	snek_offset_t	a = (snek_poly_get_soffset(addr) << 1) | 0;
	snek_list_t	*list;
	uint8_t		tmp[32];
	uint8_t		i;

	if (snek_poly_type(data) != snek_list ||
	    snek_list_type(list = snek_poly_to_list(data)) != snek_list_list ||
	    list->size > sizeof (data))
	{
		return snek_error_type_1(data);
	}
	snek_poly_t	*d = snek_list_data(list);
	for (i = 0; i < list->size; i++)
		tmp[i] = snek_poly_get_soffset(d[i]);
	if (snek_abort)
		return SNEK_NULL;
	ao_i2c_start(a);
	ao_i2c_send(tmp, list->size, true);
	return snek_float_to_poly(list->size);
}
#endif
