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

void
snek_for_add_in(snek_id_t id, uint8_t depth)
{
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
