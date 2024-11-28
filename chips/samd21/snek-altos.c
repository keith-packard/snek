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
