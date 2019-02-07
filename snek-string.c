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

char *
snek_string_make(char c)
{
	char *new = snek_alloc(2);
	if (new)
		new[0] = c;
	return new;
}

snek_poly_t
snek_string_get(char *string, snek_soffset_t o, bool report_error)
{
	if (o < 0 || strlen(string) <= (snek_offset_t) o) {
		if (report_error)
			snek_error_range(o);
		return SNEK_NULL;
	}
	return snek_string_to_poly(snek_string_make(string[o]));
}

static char *
snek_string_catn(char *a, snek_offset_t aoff, snek_offset_t alen,
		 char *b, snek_offset_t boff, snek_offset_t blen)
{
	char *new;
	if (snek_is_pool_addr(a))
		snek_string_stash(a);
	if (snek_is_pool_addr(b))
		snek_string_stash(b);
	new = snek_alloc(alen + blen + 1);
	if (snek_is_pool_addr(b))
		b = snek_string_fetch();
	if (snek_is_pool_addr(a))
		a = snek_string_fetch();
	if (new) {
		memcpy(new, a + aoff, alen);
		memcpy(new + alen, b + boff, blen);
		new[alen+blen] = '\0';
	}
	return new;
}

snek_poly_t
snek_string_cat(char *a, char *b)
{
	return snek_string_to_poly(snek_string_catn(a, 0, strlen(a),
						    b, 0, strlen(b)));
}

char *
snek_string_slice(char *a, snek_slice_t *slice)
{
	snek_string_stash(a);
	char	*r = snek_alloc(slice->len + 1);
	a = snek_string_fetch();
	if (!r)
		return NULL;
	snek_offset_t i = 0;
	for (snek_slice_start(slice); snek_slice_test(slice); snek_slice_step(slice))
		r[i++] = a[slice->pos];
	r[i] = '\0';
	return r;
}

static uint8_t
snek_next_format(char *a)
{
	char *percent = strchr(a, '%');
	if (percent)
		return percent - a;
	return strlen(a);
}


static char *
snek_buf_realloc(char **str_p, snek_offset_t add)
{
	char *old = *str_p;
	char *new;
	snek_offset_t len = old ? strlen(old) : 0;

	if (old)
		snek_string_stash(old);
	new = snek_alloc(len + add + 1);
	if (old)
		old = snek_string_fetch();
	if (!new)
		return NULL;
	memcpy(new, old, len);
	new[len+add] = '\0';
	*str_p = new;
	return new + len;
}

static int
snek_buf_sprintc(int c, void *closure)
{
	char	**str_p = closure;
	char	*new;

	if ((new = snek_buf_realloc(str_p, 1)) != NULL)
		*new = c;
	return 0;
}

static int
snek_buf_sprints(const char *s, void *closure)
{
	char	**str_p = closure;
	char	*new;
	int	len = strlen(s);
	bool	is_pool = snek_is_pool_addr(s);

	if (is_pool)
		snek_string_stash((char *) s);
	new = snek_buf_realloc(str_p, len);
	if (is_pool)
		s = snek_string_fetch();
	if (new)
		memcpy(new, s, len);
	return 0;
}

char *
snek_string_interpolate(char *a, snek_poly_t poly)
{
	uint8_t percent = 0;
	char *result = NULL;
	snek_offset_t o = 0;
	snek_buf_t buf = {
		.put_c = snek_buf_sprintc,
		.put_s = snek_buf_sprints,
		.closure = &result
	};

	while (a[percent]) {
		uint8_t next = snek_next_format(a + percent) + percent;
		snek_poly_stash(poly);
		snek_string_stash(a);
		result = snek_string_catn(result, 0, result ? strlen(result) : 0,
					  a, percent, next-percent);
		a = snek_string_fetch();
		poly = snek_poly_fetch();
		percent = next;
		if (a[percent] == '%') {
			percent++;
			char format = a[percent];
			if (format)
				percent++;
			if (format == '%')
				snek_buf_sprintc('%', &buf);
			else {
				snek_poly_t	*data = &poly;
				snek_offset_t	size = 1;
				if (snek_poly_type(poly) == snek_list) {
					snek_list_t *list = snek_poly_to_list(poly);
					data = snek_list_data(list);
					size = list->size;
				}
				snek_poly_t v = SNEK_ZERO;
				if (o < size)
					v = data[o++];
				snek_poly_stash(poly);
				snek_string_stash(a);
				snek_poly_format(&buf, v, format);
				a = snek_string_fetch();
				poly = snek_poly_fetch();
			}
		}
	}
	return result;
}

static snek_offset_t
snek_string_size(void *addr)
{
	char *string = addr;
	return (snek_offset_t) strlen(string) + 1;
}

const snek_mem_t SNEK_MEM_DECLARE(snek_string_mem) = {
	.size = snek_string_size,
	.mark = snek_null_mark_move,
	.move = snek_null_mark_move,
	SNEK_MEM_DECLARE_NAME("string")
};
