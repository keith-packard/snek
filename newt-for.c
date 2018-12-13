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

newt_range_t *newt_ranges;

void
newt_range_start(newt_id_t id, newt_offset_t nparam)
{
	newt_poly_t	current = newt_float_to_poly(0.0f);
	newt_poly_t	limit = newt_float_to_poly(0.0f);
	newt_poly_t	step = newt_float_to_poly(1.0f);

	switch (nparam) {
	case 0:
		newt_error("range needs at least one argument");
		return;
	case 1:
		limit = newt_stack_pick(0);
		break;
	case 2:
		current = newt_stack_pick(1);
		limit = newt_stack_pick(0);
		break;
	default:
		current = newt_stack_pick(2);
		limit = newt_stack_pick(1);
		step = newt_stack_pick(0);
		break;
	}

	if (!newt_is_float(current) || !newt_is_float(limit) || !newt_is_float(step)) {
		newt_error("range() requires numeric parameters");
		return;
	}

	if (newt_poly_to_float(step) == 0) {
		newt_error("range() arg 3 must not be zero");
		return;
	}

	if (!newt_id_ref(id, true))
		return;

	newt_range_t *r = newt_alloc(sizeof(newt_range_t));
	if (!r)
		return;

	r->id = id;
	r->limit = newt_poly_to_float(limit);
	r->step = newt_poly_to_float(step);
	r->current = newt_poly_to_float(current) - r->step;

	r->prev = newt_pool_offset(newt_ranges);
	newt_ranges = r;
}

/* true to continue the range */
bool
newt_range_step(void)
{
	if (!newt_ranges)
		return false;

	newt_poly_t	*ref = newt_id_ref(newt_ranges->id, false);
	if (!ref)
		goto bail;

	float c = newt_ranges->current + newt_ranges->step;

	if ((newt_ranges->step > 0) ? c < newt_ranges->limit : c > newt_ranges->limit) {
		newt_ranges->current = c;
		*ref = newt_float_to_poly(c);
		return true;
	}
bail:
	newt_ranges = newt_pool_ref(newt_ranges->prev);
	return false;
}

static int
newt_range_size(void *addr)
{
	return sizeof(newt_range_t);
}

static void
newt_range_mark(void *addr)
{
	newt_range_t *r = addr;

	for (;;) {
		r = newt_pool_ref(r->prev);
		if (!r)
			break;
		newt_mark_memory(&newt_range_mem, r);
	}
}

static void
newt_range_move(void *addr)
{
	newt_range_t *r = addr;

	for (;;) {
		if (newt_move_offset(&r->prev))
			break;
		r = newt_pool_ref(r->prev);
		if (!r)
			break;
	}
}

const newt_mem_t newt_range_mem = {
	.size = newt_range_size,
	.mark = newt_range_mark,
	.move = newt_range_move,
	.name = "range",
};

newt_in_t *newt_ins;

void
newt_in_start(newt_id_t id)
{
	newt_poly_t	array = newt_stack_pick(0);

	switch (newt_poly_type(array)) {
	case newt_list:
	case newt_string:
		break;
	default:
		newt_error("%p is not iterable", array);
		break;
	}

	/* make sure the variable is in the frame */
	if (!newt_id_ref(id, true))
		return;

	newt_in_t *i = newt_alloc(sizeof(newt_in_t));
	if (!i)
		return;

	i->id = id;
	i->array = array;
	i->i = 0;

	i->prev = newt_pool_offset(newt_ins);
	newt_ins = i;
}

bool
newt_in_step(void)
{
	if (!newt_ins)
		return false;

	newt_poly_t array = newt_ins->array;
	newt_poly_t value = NEWT_NULL;

	newt_list_t *l;
	char *s;

	switch (newt_poly_type(array)) {
	case newt_list:
		l = newt_poly_to_list(array);
		if (newt_ins->i < l->size)
			value = newt_list_data(l)[newt_ins->i];
		break;
	case newt_string:
		s = newt_poly_to_string(array);
		char c = newt_string_fetch(s, newt_ins->i);
		if (c)
			value = newt_string_to_poly(newt_string_make(c));
		break;
	default:
		goto bail;
	}
	if (newt_is_null(value))
		goto bail;

	newt_poly_t *ref = newt_id_ref(newt_ins->id, false);
	if (!ref)
		goto bail;
	*ref = value;

	++newt_ins->i;
	return true;
bail:
	newt_ins = newt_pool_ref(newt_ins->prev);
	return false;
}

static int
newt_in_size(void *addr)
{
	(void) addr;
	return sizeof(newt_in_t);
}

static void
newt_in_mark(void *addr)
{
	newt_in_t	*i = addr;

	for (;;) {
		newt_poly_mark(i->array, true);
		i = newt_pool_ref(i->prev);
		if (!i)
			break;
		newt_mark_memory(&newt_in_mem, i);
	}
}

static void
newt_in_move(void *addr)
{
	newt_in_t	*i = addr;

	for (;;) {
		newt_poly_move(&i->array, 1);
		if (newt_move_offset(&i->prev))
			break;
		i = newt_pool_ref(i->prev);
		if (!i)
			break;
	}
}

const newt_mem_t newt_in_mem = {
	.size = newt_in_size,
	.mark = newt_in_mark,
	.move = newt_in_move,
	.name = "in",
};
