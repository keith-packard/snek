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
	float current = 0.0f;
	float limit = 0.0f;
	float step = 1.0f;

	switch (nparam) {
	case 1:
		limit = newt_stack_pop_float();
		break;
	case 2:
		limit = newt_stack_pop_float();
		current = newt_stack_pop_float();
		break;
	case 3:
		step = newt_stack_pop_float();
		limit = newt_stack_pop_float();
		current = newt_stack_pop_soffset();
		if (step == 0) {
			newt_error("zero range step");
			return;
		}
		break;
	default:
		newt_error("invalid range: %d", nparam);
		newt_stack_drop(nparam);
		return;
	}

	if (!newt_id_ref(id, true))
		return;

	newt_range_t *r = newt_alloc(sizeof(newt_range_t));

	if (!r)
		return;

	r->id = id;
	r->current = current - step;
	r->limit = limit;
	r->step = step;

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

static newt_offset_t
newt_range_size(void *addr)
{
	(void) addr;
	return (newt_offset_t) sizeof(newt_range_t);
}

static void
newt_range_mark(void *addr)
{
	newt_range_t *r = addr;

	for (;;) {
		if (!r->prev)
			break;
		newt_mark_block_offset(&newt_range_mem, r->prev);
		r = newt_pool_ref(r->prev);
	}
}

static void
newt_range_move(void *addr)
{
	newt_range_t *r = addr;

	for (;;) {
		if (!r->prev)
			break;
		if (newt_move_block_offset(&r->prev))
			break;
		r = newt_pool_ref(r->prev);
	}
}

const newt_mem_t NEWT_MEM_DECLARE(newt_range_mem) = {
	.size = newt_range_size,
	.mark = newt_range_mark,
	.move = newt_range_move,
	NEWT_MEM_DECLARE_NAME("range")
};

newt_in_t *newt_ins;

void
newt_in_start(newt_id_t id)
{
	/* make sure the variable is in the frame */
	if (!newt_id_ref(id, true)) {
		newt_stack_drop(1);
		return;
	}

	newt_in_t *i = newt_alloc(sizeof(newt_in_t));
	if (!i) {
		newt_stack_drop(1);
		return;
	}

	i->id = id;
	i->array = newt_stack_pop();
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
		newt_error("not iterable: %p", array);
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

static newt_offset_t
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
		newt_poly_mark(i->array);

		if (!i->prev)
			break;
		i = newt_pool_ref(i->prev);
		newt_mark_block_addr(&newt_in_mem, i);
	}
}

static void
newt_in_move(void *addr)
{
	newt_in_t	*i = addr;

	for (;;) {
		newt_poly_move(&i->array);
		if (!i->prev)
			break;
		if (newt_move_block_offset(&i->prev))
			break;
		i = newt_pool_ref(i->prev);
	}
}

const newt_mem_t NEWT_MEM_DECLARE(newt_in_mem) = {
	.size = newt_in_size,
	.mark = newt_in_mark,
	.move = newt_in_move,
	NEWT_MEM_DECLARE_NAME("in")
};
