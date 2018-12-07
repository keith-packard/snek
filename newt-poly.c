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
newt_poly_offset(newt_offset_t offset, newt_type_t type)
{
	return newt_uint_to_value(offset | type);
}

newt_poly_t
newt_poly(const void *addr, newt_type_t type)
{
	return newt_poly_offset((const uint8_t *) addr - newt_pool, type);
}

void
newt_poly_print(newt_poly_t poly)
{
	switch (newt_poly_type(poly)) {
	case newt_float:
		printf("%g", newt_poly_to_float(poly));
		break;
	case newt_string:
		printf("\"%s\"", newt_poly_to_string(poly));
		break;
	case newt_func:
		printf("<function at %d>", newt_poly_to_uint(poly));
		break;
	default:
		printf("?%d.%x?", newt_poly_type(poly), newt_poly_to_uint(poly));
		break;
	}
}

bool
newt_slice_canon(newt_slice_t *slice)
{
	if (slice->start == NEWT_SLICE_DEFAULT)
		slice->start = 0;
	if (slice->stride == NEWT_SLICE_DEFAULT)
		slice->stride = 1;
	if (slice->end == NEWT_SLICE_DEFAULT) {
		if (slice->stride < 0)
			slice->end = -slice->len - 1;
		else
			slice->end = slice->len;
	}
	if (slice->start < 0) {
		slice->start = slice->len + slice->start;
		if (slice->start < 0)
			slice->start = 0;
	}
	if (slice->end < 0) {
		slice->end = slice->len + slice->end;
	}
	if (slice->start > slice->len)
		return false;
	if (slice->end > slice->len)
		return false;
	if (slice->stride == 0)
		return false;
	slice->count = (slice->end - slice->start) / abs(slice->stride);
	return true;
}

void
newt_null_mark(void *addr)
{
	(void) addr;
}

void
newt_null_move(void *addr)
{
	(void) addr;
}
