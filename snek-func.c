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

snek_code_t		*snek_stash_code;

snek_func_t *
snek_func_alloc(snek_code_t *code)
{
	snek_func_t *func;

	snek_stash_code = code;
	func = snek_alloc(sizeof (snek_func_t) + snek_parse_nformal * sizeof (snek_id_t));
	code = snek_stash_code;
	snek_stash_code = NULL;
	if (!func)
		return NULL;
	func->code = snek_pool_offset(code);
	func->nformal = snek_parse_nformal;
	func->nrequired = snek_parse_nformal - snek_parse_nnamed;
	memcpy(func->formals, snek_parse_formals, snek_parse_nformal * sizeof (snek_id_t));
	return func;
}

/*
 * Assign a function formal from an actual value.
 *
 * Check to make sure there aren't duplicate assignments,
 * e.g. a positional actual and named actual that end up
 * naming the same formal.
 */
static bool __attribute__((noinline))
snek_func_actual(snek_id_t id, snek_poly_t value, uint8_t pos)
{
	snek_variable_t *existing = &snek_frame->variables[snek_frame->nvariables-1];
	snek_variable_t *insert = &snek_frame->variables[pos];

	/* Check existing formals to see if we're duplicating one of
	 * those
	 */
	while (existing > insert) {
		if (existing->id == id)
			return false;
		existing--;
	}

	/* All good, assign the name and value in the frame */
	existing->id = id;
	existing->value = value;
	return true;
}

/*
 * Make sure a named actual matches a known formal
 */
static bool
snek_func_check_formal(snek_id_t id, snek_func_t *func)
{
	for (uint8_t f = 0; f < func->nformal; f++)
		if (func->formals[f] == id)
			return true;
	return false;
}

/*
 * Create a new frame holding all of the actuals
 */
bool
snek_func_push(uint8_t nposition, uint8_t nnamed, snek_offset_t ip)
{
	/* Allocate the frame first so that
	 * nothing moves during the rest of the function
	 */
	uint8_t nparam = nposition + nnamed;
	if (!snek_frame_push(ip, nparam))
		return false;

	snek_func_t *func = snek_poly_to_func(snek_a);

	/* Check to make sure we don't pass more actuals by position
	 * than there are formals in the function
	 */
	if (nposition > func->nformal) {
		snek_error_args(func->nformal, nposition);
		goto fail_frame;
	}

	uint8_t pos = nparam;
	snek_offset_t save_stackp = snek_stackp;
	snek_poly_t value;
	snek_id_t id;

	/* Assign formals from actuals */
	while (pos) {
		pos--;
		value = snek_stack_pop();

		if (pos >= nposition) {
			/* named actuals come last */
			id = snek_stack_pop_soffset();

			/* Make sure the actual matches a formal */
			if (!snek_func_check_formal(id, func))
				goto fail;
		} else {
			/* positional actuals come first */
			id = func->formals[pos];
		}

		/* Insert into the frame, checking for duplicates */
		if (!snek_func_actual(id, value, pos))
			goto fail;
	}

	/* Make sure all required formals were given values */
	for (pos = 0; pos < func->nrequired; pos++)
		if (!snek_frame->variables[pos].id) {
			id = func->formals[pos];
			goto fail;
		}

	return true;
fail:
	/* Restore stack and complain about the invalid actual */
	snek_stackp = save_stackp;
	snek_error_arg(id);
fail_frame:
	/* Remove the invalid frame */
	(void) snek_frame_pop();
	return false;
}

snek_offset_t
snek_func_size(void *addr)
{
	snek_func_t *func = addr;

	return (snek_offset_t) sizeof (snek_func_t) + func->nformal * (snek_offset_t) sizeof (snek_id_t);
}

void
snek_func_mark(void *addr)
{
	snek_func_t *func = addr;

	snek_mark_offset(&snek_code_mem, func->code);
}

void
snek_func_move(void *addr)
{
	snek_func_t *func = addr;

	snek_move_offset(&snek_code_mem, &func->code);
}
