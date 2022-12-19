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
#include <math.h>

#ifdef SNEK_BUILTIN_input
bool snek_in_input;

snek_poly_t
snek_builtin_input(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	char		in[2] = {0, 0};
	int		c;
	snek_poly_t	s = snek_string_to_poly(snek_alloc(1));

	/* print out an optional parameter */
	if (nposition) {
		if (nposition > 1)
			return snek_error_args(1, nposition);
		snek_poly_print(stdout, *args, 's');
	}
	(void) nnamed;
	snek_in_input = true;
	while ((c = getchar()) != '\n' && c != EOF) {
		in[0] = c;
		s = snek_string_cat(snek_poly_to_string(s), in);
	}
	snek_in_input = false;
	return s;
}
#endif

#if defined(SNEK_BUILTIN_float)
snek_poly_t
snek_builtin_float(snek_poly_t a)
{
	float f;
	switch (snek_poly_type(a)) {
	case snek_string:
		f = strtof(snek_poly_to_string(a), NULL);
		break;
	case snek_float:
		f = snek_poly_to_float(a);
		break;
	default:
		return snek_error_type_1(a);
	}
	return snek_float_to_poly(f);
}
#endif

#ifdef SNEK_BUILTIN_int
snek_poly_t
snek_builtin_int(snek_poly_t a)
{
	return snek_float_to_poly(truncf(snek_poly_get_float(snek_builtin_float(a))));
}
#endif
