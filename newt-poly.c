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
	return newt_pool + newt_poly_to_offset(poly);
}

newt_poly_t
newt_poly(const void *addr, newt_type_t type)
{
	if (addr == NULL)
		return NEWT_ZERO;
	return newt_offset_to_poly((const uint8_t *) addr - newt_pool, type);
}

static inline newt_offset_t
newt_func_line(newt_func_t *func)
{
	newt_code_t	*code = newt_pool_ref(func->code);
	if (code)
		return newt_code_line(code);
	return 0;
}

void
newt_poly_print(FILE *file, newt_poly_t poly, char format)
{
	newt_buf_t buf = {
		.put_c = (int(*) (int, void *)) fputc,
		.put_s = (int(*) (const char *, void *)) fputs,
		.closure = file
	};
	newt_poly_format(&buf, poly, format);
}

bool
newt_poly_equal(newt_poly_t a, newt_poly_t b)
{
	if (a.u == b.u)
		return true;
	newt_type_t atype = newt_poly_type(a);
	newt_type_t btype = newt_poly_type(b);
	if (atype != btype)
		return false;
	switch (atype) {
	case newt_string:
		return !strcmp(newt_poly_to_string(a), newt_poly_to_string(b));
	case newt_list:
		return newt_list_equal(newt_poly_to_list(a), newt_poly_to_list(b));
	default:
		return false;
	}
}

bool
newt_poly_true(newt_poly_t a)
{
	switch (newt_poly_type(a)) {
	case newt_float:
		return newt_poly_to_float(a) != 0.0f;
	case newt_list:
		return newt_poly_to_list(a)->size != 0;
	case newt_string:
		return strlen(newt_poly_to_string(a)) != 0;
	default:
		return false;
	}
}

int
newt_poly_len(newt_poly_t a)
{
	switch (newt_poly_type(a)) {
	case newt_string:
		return strlen(newt_poly_to_string(a));
	case newt_list:
		return newt_poly_to_list(a)->size;
	default:
		return 0;
	}
}

newt_offset_t
newt_null_size(void *addr)
{
	(void) addr;
	return 0;
}

void
newt_null_mark_move(void *addr)
{
	(void) addr;
}

const newt_mem_t NEWT_MEM_DECLARE(newt_null_mem) = {
	.size = newt_null_size,
	.mark = newt_null_mark_move,
	.move = newt_null_mark_move,
	NEWT_MEM_DECLARE_NAME("null")
};
