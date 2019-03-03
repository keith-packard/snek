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
#include <math.h>

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
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_sys_stdout_flush(void)
{
	fflush(stdout);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_ord(snek_poly_t a)
{
	if (snek_poly_type(a) != snek_string)
		return snek_error_type_1(a);
	return snek_float_to_poly(snek_poly_to_string(a)[0]);
}

snek_poly_t
snek_builtin_chr(snek_poly_t a)
{
	snek_soffset_t s = snek_poly_get_soffset(a);
	return snek_string_to_poly(snek_string_make(s));
}

snek_poly_t
snek_builtin_math_sqrt(snek_poly_t a)
{
	return snek_float_to_poly(sqrtf(snek_poly_get_float(a)));
}
