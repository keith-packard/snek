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

char *
newt_string_copy(char *string)
{
	char *new;
	newt_poly_stash(newt_string_to_poly(string));
	new = newt_alloc(strlen(string) + 1);
	string = newt_poly_to_string(newt_poly_fetch());
	if (new)
		strcpy(new, string);
	return new;
}

char *
newt_string_make(char c)
{
	char *new = newt_alloc(2);
	if (new)
		new[0] = c;
	return new;
}

char
newt_string_fetch(char *string, int i)
{
	if (i < strlen(string))
		return string[i];
	return '\0';
}

char *
newt_string_cat(char *a, char *b)
{
	char *new;
	newt_poly_stash(newt_string_to_poly(a));
	newt_poly_stash(newt_string_to_poly(b));
	new = newt_alloc(strlen(a) + strlen(b) + 1);
	b = newt_poly_to_string(newt_poly_fetch());
	a = newt_poly_to_string(newt_poly_fetch());
	if (new) {
		strcpy(new, a);
		strcat(new, b);
	}
	return new;
}

char *
newt_string_slice(char *a, newt_slice_t *slice)
{
	newt_poly_stash(newt_string_to_poly(a));
	char	*r = newt_alloc(slice->len + 1);
	a = newt_poly_to_string(newt_poly_fetch());
	if (!r)
		return NULL;
	newt_offset_t i = 0;
	for (newt_slice_start(slice); newt_slice_test(slice); newt_slice_step(slice))
		r[i++] = a[slice->pos];
	r[i] = '\0';
	return r;
}

int
newt_string_size(void *addr)
{
	char *string = addr;
	return strlen(string) + 1;
}

const newt_mem_t newt_string_mem = {
	.size = newt_string_size,
	.mark = newt_null_mark,
	.move = newt_null_move,
	.name = "string"
};
