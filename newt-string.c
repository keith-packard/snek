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
newt_string_make(char c)
{
	char *new = newt_alloc(2);
	if (new)
		new[0] = c;
	return new;
}

char
newt_string_get(char *string, int i)
{
	if (i < (int) strlen(string))
		return string[i];
	return '\0';
}

static char *
newt_string_catn(char *a, newt_offset_t aoff, newt_offset_t alen,
		 char *b, newt_offset_t boff, newt_offset_t blen)
{
	char *new;
	if (newt_is_pool_addr(a))
		newt_poly_stash(newt_string_to_poly(a));
	if (newt_is_pool_addr(b))
		newt_poly_stash(newt_string_to_poly(b));
	new = newt_alloc(alen + blen + 1);
	if (newt_is_pool_addr(b))
		b = newt_poly_to_string(newt_poly_fetch());
	if (newt_is_pool_addr(a))
		a = newt_poly_to_string(newt_poly_fetch());
	if (new) {
		memcpy(new, a + aoff, alen);
		memcpy(new + alen, b + boff, blen);
		new[alen+blen] = '\0';
	}
	return new;
}

char *
newt_string_cat(char *a, char *b)
{
	return newt_string_catn(a, 0, strlen(a),
				b, 0, strlen(b));
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

static uint8_t
newt_next_format(char *a)
{
	char *percent = strchr(a, '%');
	if (percent)
		return percent - a;
	return strlen(a);
}


static char *
newt_buf_realloc(char **str_p, newt_offset_t add)
{
	char *old = *str_p;
	char *new;
	newt_offset_t len = old ? strlen(old) : 0;

	if (old)
		newt_string_stash(old);
	new = newt_alloc(len + add + 1);
	if (old)
		old = newt_string_fetch();
	if (!new)
		return NULL;
	memcpy(new, old, len);
	new[len+add] = '\0';
	*str_p = new;
	return new + len;
}

static int
newt_buf_sprintc(int c, void *closure)
{
	char	**str_p = closure;
	char	*new;

	if ((new = newt_buf_realloc(str_p, 1)) != NULL) {
		*new = c;
		return c;
	}
	return EOF;
}

static int
newt_buf_sprints(const char *s, void *closure)
{
	char	**str_p = closure;
	char	*new;
	int	len = strlen(s);
	bool	is_pool = newt_is_pool_addr(s);

	if (is_pool)
		newt_string_stash((char *) s);
	new = newt_buf_realloc(str_p, len);
	if (is_pool)
		s = newt_string_fetch();
	if (new) {
		memcpy(new, s, len);
		return len;
	}
	return EOF;
}

char *
newt_string_interpolate(char *a, newt_poly_t poly)
{
	uint8_t percent = 0;
	char *result = NULL;
	newt_offset_t o = 0;
	newt_buf_t buf = {
		.put_c = newt_buf_sprintc,
		.put_s = newt_buf_sprints,
		.closure = &result
	};

	while (a[percent]) {
		uint8_t next = newt_next_format(a + percent) + percent;
		newt_poly_stash(poly);
		newt_string_stash(a);
		result = newt_string_catn(result, 0, result ? strlen(result) : 0,
					  a, percent, next-percent);
		a = newt_string_fetch();
		poly = newt_poly_fetch();
		percent = next;
		if (a[percent] == '%') {
			percent++;
			char format = a[percent];
			if (format)
				percent++;
			if (format == '%')
				newt_buf_sprintc('%', &buf);
			else {
				newt_poly_t	*data = &poly;
				newt_offset_t	size = 1;
				if (newt_poly_type(poly) == newt_list) {
					newt_list_t *list = newt_poly_to_list(poly);
					data = newt_list_data(list);
					size = list->size;
				}
				newt_poly_t v = NEWT_ZERO;
				if (o < size)
					v = data[o++];
				newt_poly_format(&buf, v, format);
			}
		}
	}
	return result;
}

newt_offset_t
newt_string_size(void *addr)
{
	char *string = addr;
	return (newt_offset_t) strlen(string) + 1;
}

const newt_mem_t NEWT_MEM_DECLARE(newt_string_mem) = {
	.size = newt_string_size,
	.mark = newt_null_mark_move,
	.move = newt_null_mark_move,
	NEWT_MEM_DECLARE_NAME("string")
};
