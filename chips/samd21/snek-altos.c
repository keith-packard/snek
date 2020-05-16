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

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	uint64_t	ticks = (snek_poly_get_float(a) * 1e9f + 0.5f);
	uint64_t	expire = ao_time_ns() + ticks;

	while (!snek_abort && (int64_t) (expire - ao_time_ns()) > 0)
		;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly(ao_time_ns() / 1e9f);
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
	return snek_float_to_poly(((snek_soffset_t) (random_x % snek_poly_get_soffset(a))));
}
