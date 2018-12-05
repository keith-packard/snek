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

void *
newt_ref(newt_poly_t poly)
{
	if (newt_is_null(poly))
		return NULL;
	return newt_pool + (poly.u & 0x0fffffc);
}

newt_poly_t
newt_poly(const void *addr, newt_type_t type)
{
	return newt_uint_to_value(((const uint8_t *) addr - newt_pool) | type);
}

