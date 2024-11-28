/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
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

#include "snek.h"

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

static void
next_random(void)
{
	random_x *= random_x;
	random_w += random_s;
	random_x += random_w;
	random_x = (random_x >> 32) | (random_x << 32);
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	uint32_t mod = snek_poly_get_float(a);

	if (!mod) {
		snek_error_value(a);
		return SNEK_NULL;
	}
	next_random();
	return snek_float_to_poly(random_x % mod);
}

snek_poly_t
snek_builtin_random_random(void)
{
	next_random();
	return snek_float_to_poly(((uint32_t) random_x) / 0x1p+32);
}
