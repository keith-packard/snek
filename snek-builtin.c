/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
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

#include "snek.h"

snek_poly_t
snek_builtin_len(snek_poly_t a)
{
	return snek_float_to_poly(snek_poly_len(a));
}

snek_poly_t
snek_builtin_print(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	while (nposition--) {
		snek_poly_t arg = *args++;
		snek_poly_print(stdout, arg, 's');
	}
	snek_poly_t end = SNEK_NULL;

	while (nnamed--) {
		snek_id_t id = (snek_id_t) ((*args++).f);
		snek_poly_t value = *args++;
		if (id == SNEK_BUILTIN_end)
			end = value;
	}
	if (!snek_is_null(end))
		snek_poly_print(stdout, end, 's');
	else
		putc('\n', stdout);
	return SNEK_ZERO;
}

snek_poly_t
snek_builtin_sys_stdout_flush(void)
{
	fflush(stdout);
	return SNEK_ONE;
}
