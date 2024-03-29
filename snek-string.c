/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
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

snek_poly_t
snek_string_make(char c)
{
	char *new = snek_alloc(2);
	if (new)
		new[0] = c;
	return snek_string_to_poly(new);
}

#ifdef SNEK_STRING_BUILD
snek_poly_t
snek_string_build(const char *s)
{
	char *new = snek_alloc(strlen(s) + 1);
	if (new)
		strcpy(new, s);
	return snek_string_to_poly(new);
}
#endif

snek_poly_t
snek_string_get(char *string, snek_poly_t p, bool report_error)
{
	snek_soffset_t so = snek_poly_get_soffset(p);
	snek_offset_t len = strlen(string);
	snek_offset_t o;

	o = (snek_offset_t) so;
	if (so < 0)
		o = len - (-so);
	if (len <= o) {
		if (report_error)
			snek_error_value(p);
		return SNEK_NULL;
	}
	return snek_string_make(string[o]);
}

static char *
snek_string_catn(char *a, snek_offset_t aoff, snek_offset_t alen,
		 char *b, snek_offset_t boff, snek_offset_t blen)
{
	char *new;
	snek_stack_push_string(a);
	snek_stack_push_string(b);
	new = snek_alloc(alen + blen + 1);
	b = snek_stack_pop_string(b);
	a = snek_stack_pop_string(a);
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

#ifndef SNEK_NO_SLICE
char *
snek_string_slice(char *a, snek_slice_t *slice)
{
	if (slice->identity)
		return a;

	snek_stack_push_string(a);
	char	*r = snek_alloc(slice->count + 1);
	a = snek_stack_pop_string(a);
	if (!r)
		return NULL;
	snek_offset_t i = 0;
	for (; snek_slice_test(slice); snek_slice_step(slice))
		r[i++] = a[slice->pos];
	r[i] = '\0';
	return r;
}
#endif

snek_poly_t
snek_string_times(char *a, snek_soffset_t b)
{
	snek_offset_t alen = strlen(a);
	char *s = snek_alloc(alen * b + 1);
	if (s) {
		char *t = s;
		while (b--) {
			memcpy(t, a, alen);
			t += alen;
		}
		*t = '\0';
	}
	return snek_string_to_poly(s);
}

static snek_offset_t
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

	snek_stack_push_string(old);
	new = snek_alloc(len + add + 1);
	old = snek_stack_pop_string(old);
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

	snek_stack_push_string(s);
	new = snek_buf_realloc(str_p, len);
	s = snek_stack_pop_string(s);
	if (new)
		memcpy(new, s, len);
	return 0;
}

snek_poly_t
snek_string_interpolate(char *a, snek_poly_t poly)
{
	snek_offset_t percent = 0;
	char *result = NULL;
	snek_offset_t o = 0;
	bool is_list;
	snek_offset_t size;
	snek_buf_t buf = {
		.put_c = snek_buf_sprintc,
		.put_s = snek_buf_sprints,
		.closure = &result
	};

	size = 1;
	is_list = false;
	if (snek_poly_type(poly) == snek_list) {
		snek_list_t *list = snek_poly_to_list(poly);
#ifndef SNEK_NO_DICT
		if (snek_list_type(list) != snek_list_dict)
#endif
		{
			is_list = true;
			size = list->size;
		}
	}

	while (a[percent]) {
		snek_offset_t next = snek_next_format(a + percent) + percent;
		snek_stack_push(poly);
		snek_stack_push_string(a);
		result = snek_string_catn(result, 0, result ? strlen(result) : 0,
					  a, percent, next-percent);
		a = snek_stack_pop_string(a);
		poly = snek_stack_pop();
		percent = next;
		if (a[percent] == '%') {
			percent++;
			char format = a[percent];
			snek_stack_push(poly);
			snek_stack_push_string(a);
			if (format == '\0') {
				o = size + 1;
			} else {
				percent++;
				if (format == '%') {
					snek_buf_sprintc('%', &result);
				} else {
					snek_poly_t	*data = &poly;

					if (is_list)
						data = snek_list_data(snek_poly_to_list(poly));
					if (o >= size)
						o = size + 1;
					else
						snek_poly_format(&buf, data[o++], format);
				}
			}
			a = snek_stack_pop_string(a);
			poly = snek_stack_pop();
		}
	}
	if (o != size)
		return SNEK_INVALID;
	return snek_string_to_poly(result);
}

#ifdef SNEK_BUILTIN_str
snek_poly_t
snek_builtin_str(snek_poly_t a)
{
	char *result = NULL;
	snek_buf_t buf = {
		.put_c = snek_buf_sprintc,
		.put_s = snek_buf_sprints,
		.closure = &result
	};
	snek_poly_format(&buf, a, 's');
	return snek_string_to_poly(result);
}
#endif

void
snek_stack_push_string(const char *s)
{
	if (snek_is_pool_addr(s))
		snek_stack_push(snek_string_to_poly((char *) s));
}

char *
snek_stack_pop_string(const char *s)
{
	if (snek_is_pool_addr(s))
		return snek_poly_to_string(snek_stack_pop());
	return (char *) s;
}

snek_offset_t
snek_string_size(void *addr)
{
	char *string = addr;
	return (snek_offset_t) strlen(string) + 1;
}

void
snek_string_mark_move(void *addr)
{
	(void) addr;
}
