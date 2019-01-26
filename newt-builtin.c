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
newt_builtin_printn(newt_poly_t a)
{
	if (newt_poly_type(a) == newt_string)
		fputs(newt_poly_to_string(a), stdout);
	else
		newt_poly_print(stdout, a);
	return NEWT_ZERO;
}

newt_poly_t
newt_builtin_print(newt_poly_t a)
{
	newt_builtin_printn(a);
	putc('\n', stdout);
	return NEWT_ZERO;
}

newt_poly_t
newt_builtin_sys_stdout_flush(void)
{
	fflush(stdout);
	return NEWT_ONE;
}
