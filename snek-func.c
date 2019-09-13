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

static bool __attribute__((noinline))
snek_func_actual(snek_id_t id, snek_poly_t value, uint8_t pos)
{
	snek_variable_t *existing = &snek_frame->variables[snek_frame->nvariables-1];
	snek_variable_t *insert = &snek_frame->variables[pos];

	while (existing > insert) {
		if (existing->id == id)
			return false;
		existing--;
	}
	existing->id = id;
	existing->value = value;
	return true;
}

static bool
snek_func_check_formal(snek_id_t id, snek_func_t *func)
{
	for (uint8_t f = 0; f < func->nformal; f++)
		if (func->formals[f] == id)
			return true;
	return false;
}

bool
snek_func_push(uint8_t nposition, uint8_t nnamed, snek_offset_t ip)
{
	uint8_t nparam = nposition + nnamed;
	if (!snek_frame_push(ip, nparam))
		return false;

	snek_func_t *func = snek_poly_to_func(snek_a);

	if (nposition > func->nformal) {
		snek_error_args(func->nformal, nposition);
		return false;
	}

	uint8_t pos = nparam;
	snek_offset_t save_stackp = snek_stackp;
	snek_poly_t value;
	snek_id_t id;

	/* Pop the named actuals off the stack, assigning by name */
	while (pos > nposition) {
		pos--;
		value = snek_stack_pop();
		id = snek_stack_pop_soffset();
		if (!snek_func_check_formal(id, func))
			goto fail;

		if (!snek_func_actual(id, value, pos))
			goto fail;
	}

	/* Pop the positional actuals off the stack, assigning in reverse order */
	while (pos) {
		pos--;
		value = snek_stack_pop();
		id = func->formals[pos];

		if (!snek_func_actual(id, value, pos))
			goto fail;
	}

	for (pos = 0; pos < func->nrequired; pos++)
		if (!snek_frame->variables[pos].id) {
			id = func->formals[pos];
			goto fail;
		}

	return true;
fail:
	snek_stackp = save_stackp;
	snek_error_arg(id);
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
