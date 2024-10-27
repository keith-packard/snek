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

#include "snek.h"

static uint32_t random_next;

snek_poly_t
snek_builtin_random_seed(snek_poly_t a)
{
	random_next = a.u;
	return SNEK_NULL;
}

static void
next_random(void)
{
	random_next = random_next * 1103515245L + 12345L;
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	next_random();
	return snek_float_to_poly(random_next % (uint32_t) snek_poly_get_float(a));
}

snek_poly_t
snek_builtin_random_random(void)
{
	next_random();
	return snek_float_to_poly(random_next / 0x1p+31f);
}
