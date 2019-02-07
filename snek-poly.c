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

void *
snek_ref(snek_poly_t poly)
{
	if (snek_is_null(poly))
		return NULL;
	return snek_pool + snek_poly_to_offset(poly);
}

snek_poly_t
snek_poly(const void *addr, snek_type_t type)
{
	if (addr == NULL)
		return SNEK_ZERO;
	return snek_offset_to_poly((const uint8_t *) addr - snek_pool, type);
}

snek_poly_t
snek_float_to_poly(float f)
{
	if (isnanf(f))
		return SNEK_NAN;
	return (snek_poly_t) { .f = f };
}

snek_type_t
snek_poly_type(snek_poly_t v)
{
	return snek_is_float(v) ? snek_float : (v.u & 3);
}

void
snek_poly_print(FILE *file, snek_poly_t poly, char format)
{
	snek_buf_t buf = {
		.put_c = (int(*) (int, void *)) fputc,
		.put_s = (int(*) (const char *, void *)) fputs,
		.closure = file
	};
	snek_poly_format(&buf, poly, format);
}

bool
snek_poly_equal(snek_poly_t a, snek_poly_t b, bool is)
{
	if (a.u == b.u)
		return true;
	snek_type_t atype = snek_poly_type(a);
	snek_type_t btype = snek_poly_type(b);
	if (atype != btype)
		return false;
	switch (atype) {
	case snek_string:
		return !strcmp(snek_poly_to_string(a), snek_poly_to_string(b));
	case snek_list:
		return snek_list_equal(snek_poly_to_list(a), snek_poly_to_list(b), is);
	default:
		return false;
	}
}

bool
snek_poly_true(snek_poly_t a)
{
	switch (snek_poly_type(a)) {
	case snek_float:
		return snek_poly_to_float(a) != 0.0f;
	case snek_list:
		return snek_poly_to_list(a)->size != 0;
	case snek_string:
		return strlen(snek_poly_to_string(a)) != 0;
	default:
		return false;
	}
}

snek_offset_t
snek_poly_len(snek_poly_t a)
{
	switch (snek_poly_type(a)) {
	case snek_string:
		return strlen(snek_poly_to_string(a));
	case snek_list:
		return snek_poly_to_list(a)->size;
	default:
		return 0;
	}
}

snek_offset_t
snek_null_size(void *addr)
{
	(void) addr;
	return 0;
}

void
snek_null_mark_move(void *addr)
{
	(void) addr;
}

const snek_mem_t SNEK_MEM_DECLARE(snek_null_mem) = {
	.size = snek_null_size,
	.mark = snek_null_mark_move,
	.move = snek_null_mark_move,
	SNEK_MEM_DECLARE_NAME("null")
};
