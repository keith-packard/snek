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

#include "newt.h"

newt_poly_t
newt_builtin_len(newt_poly_t a)
{
	return newt_float_to_poly(newt_poly_len(a));
}

newt_poly_t
newt_builtin_print(uint8_t nposition, uint8_t nnamed, newt_poly_t *args)
{
	while (nposition--) {
		newt_poly_t arg = *args++;
		newt_poly_print(stdout, arg, 's');
	}
	newt_poly_t end = NEWT_NULL;

	while (nnamed--) {
		newt_id_t id = (newt_id_t) ((*args++).f);
		newt_poly_t value = *args++;
		if (id == NEWT_BUILTIN_end)
			end = value;
	}
	if (!newt_is_null(end))
		newt_poly_print(stdout, end, 's');
	else
		putc('\n', stdout);
	return NEWT_ZERO;
}

newt_poly_t
newt_builtin_sys_stdout_flush(void)
{
	fflush(stdout);
	return NEWT_ONE;
}
