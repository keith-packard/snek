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

snek_range_t *snek_ranges;

void
snek_range_start(snek_id_t id, snek_offset_t nparam)
{
	float current = 0.0f;
	float limit = 0.0f;
	float step = 1.0f;

	switch (nparam) {
	case 1:
		limit = snek_stack_pop_float();
		break;
	case 2:
		limit = snek_stack_pop_float();
		current = snek_stack_pop_float();
		break;
	case 3:
		step = snek_stack_pop_float();
		limit = snek_stack_pop_float();
		current = snek_stack_pop_soffset();
		if (step == 0) {
			snek_error("zero range step");
			return;
		}
		break;
	default:
		snek_error("invalid range: %d", nparam);
		snek_stack_drop(nparam);
		return;
	}

	if (!snek_id_ref(id, true))
		return;

	snek_range_t *r = snek_alloc(sizeof(snek_range_t));

	if (!r)
		return;

	r->id = id;
	r->current = current - step;
	r->limit = limit;
	r->step = step;

	r->prev = snek_pool_offset(snek_ranges);
	snek_ranges = r;
}

/* true to continue the range */
bool
snek_range_step(void)
{
	if (!snek_ranges)
		return false;

	snek_poly_t	*ref = snek_id_ref(snek_ranges->id, false);
	if (!ref)
		goto bail;

	float c = snek_ranges->current + snek_ranges->step;

	if ((snek_ranges->step > 0) ? c < snek_ranges->limit : c > snek_ranges->limit) {
		snek_ranges->current = c;
		*ref = snek_float_to_poly(c);
		return true;
	}
bail:
	snek_ranges = snek_pool_ref(snek_ranges->prev);
	return false;
}

static snek_offset_t
snek_range_size(void *addr)
{
	(void) addr;
	return (snek_offset_t) sizeof(snek_range_t);
}

static void
snek_range_mark(void *addr)
{
	snek_range_t *r = addr;

	for (;;) {
		if (!r->prev)
			break;
		snek_mark_block_offset(&snek_range_mem, r->prev);
		r = snek_pool_ref(r->prev);
	}
}

static void
snek_range_move(void *addr)
{
	snek_range_t *r = addr;

	for (;;) {
		if (!r->prev)
			break;
		if (snek_move_block_offset(&r->prev))
			break;
		r = snek_pool_ref(r->prev);
	}
}

const snek_mem_t SNEK_MEM_DECLARE(snek_range_mem) = {
	.size = snek_range_size,
	.mark = snek_range_mark,
	.move = snek_range_move,
	SNEK_MEM_DECLARE_NAME("range")
};

snek_in_t *snek_ins;

void
snek_in_start(snek_id_t id)
{
	/* make sure the variable is in the frame */
	if (!snek_id_ref(id, true)) {
		snek_stack_drop(1);
		return;
	}

	snek_in_t *i = snek_alloc(sizeof(snek_in_t));
	if (!i) {
		snek_stack_drop(1);
		return;
	}

	i->id = id;
	i->array = snek_stack_pop();
	i->i = 0;

	i->prev = snek_pool_offset(snek_ins);
	snek_ins = i;
}

bool
snek_in_step(void)
{
	if (!snek_ins)
		return false;

	snek_poly_t array = snek_ins->array;
	snek_poly_t value = SNEK_NULL;

	snek_list_t *l;
	char *s;

	switch (snek_poly_type(array)) {
	case snek_list:
		l = snek_poly_to_list(array);
		if (snek_ins->i < l->size)
			value = snek_list_data(l)[snek_ins->i];
		break;
	case snek_string:
		s = snek_poly_to_string(array);
		char c = snek_string_get(s, snek_ins->i);
		if (c)
			value = snek_string_to_poly(snek_string_make(c));
		break;
	default:
		snek_error("not iterable: %p", array);
		goto bail;
	}
	if (snek_is_null(value))
		goto bail;

	snek_poly_t *ref = snek_id_ref(snek_ins->id, false);
	if (!ref)
		goto bail;
	*ref = value;

	++snek_ins->i;
	return true;
bail:
	snek_ins = snek_pool_ref(snek_ins->prev);
	return false;
}

static snek_offset_t
snek_in_size(void *addr)
{
	(void) addr;
	return sizeof(snek_in_t);
}

static void
snek_in_mark(void *addr)
{
	snek_in_t	*i = addr;

	for (;;) {
		snek_poly_mark(i->array);

		if (!i->prev)
			break;
		i = snek_pool_ref(i->prev);
		snek_mark_block_addr(&snek_in_mem, i);
	}
}

static void
snek_in_move(void *addr)
{
	snek_in_t	*i = addr;

	for (;;) {
		snek_poly_move(&i->array);
		if (!i->prev)
			break;
		if (snek_move_block_offset(&i->prev))
			break;
		i = snek_pool_ref(i->prev);
	}
}

const snek_mem_t SNEK_MEM_DECLARE(snek_in_mem) = {
	.size = snek_in_size,
	.mark = snek_in_mark,
	.move = snek_in_move,
	SNEK_MEM_DECLARE_NAME("in")
};
