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

snek_func_t *
snek_func_alloc(snek_code_t *code)
{
	snek_func_t *func;

	snek_code_stash(code);
	func = snek_alloc(sizeof (snek_func_t) + snek_parse_nformal * sizeof (snek_id_t));
	code = snek_code_fetch();
	if (!func)
		return NULL;
	func->code = snek_pool_offset(code);
	func->nformal = snek_parse_nformal;
	memcpy(func->formals, snek_parse_formals, snek_parse_nformal * sizeof (snek_id_t));
	return func;
}

bool
snek_func_push(uint8_t nposition, uint8_t nnamed, snek_offset_t ip)
{
	uint8_t nparam = nposition + nnamed;
	if (!snek_frame_push(ip, nparam))
		return false;

	snek_func_t *func = snek_poly_to_func(snek_a);

	if (nposition != func->nformal)
	{
		snek_error("wrong number of args: wanted %d, got %d", func->nformal, nposition);
		return false;
	}

	snek_variable_t *v = &snek_frame->variables[nparam];

	while (nnamed--) {
		v--;
		v->value = snek_stack_pop();
		v->id = snek_stack_pop_soffset();
	}
	/* Pop the arguments off the stack, assigning in reverse order */
	while (nposition--) {
		v--;
		v->id = func->formals[nposition];
		v->value = snek_stack_pop();
	}
	return true;
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

	if (func->code)
		snek_mark_offset(&snek_code_mem, func->code);
}

void
snek_func_move(void *addr)
{
	snek_func_t *func = addr;

	if (func->code)
		snek_move_offset(&snek_code_mem, &func->code);
}
