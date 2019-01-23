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

newt_func_t *
newt_func_alloc(newt_code_t *code, newt_offset_t nformal, newt_id_t *formals)
{
	newt_func_t *func;

	newt_code_stash(code);
	func = newt_alloc(sizeof (newt_func_t) + nformal * sizeof (newt_id_t));
	code = newt_code_fetch();
	if (!func)
		return NULL;
	func->code = newt_pool_offset(code);
	func->nformal = nformal;
	memcpy(func->formals, formals, nformal * sizeof (newt_id_t));
	return func;
}

bool
newt_func_push(newt_func_t *func, newt_offset_t nactual, newt_code_t *code, newt_offset_t ip)
{
	if (nactual != func->nformal) {
		newt_error("wrong number of args: wanted %d, got %d", func->nformal, nactual);
		return false;
	}

	newt_poly_stash(newt_func_to_poly(func));
	newt_frame_t *frame = newt_frame_push(code, ip, func->nformal);
	func = newt_poly_to_func(newt_poly_fetch());
	if (!frame)
		return false;

	/* Pop the arguments off the stack, assigning in reverse order */
	while (nactual) {
		newt_variable_t *v = &frame->variables[--nactual];
		v->id = func->formals[nactual];
		v->value = newt_stack_pop();
	}
	return true;
}

static newt_offset_t
newt_func_size(void *addr)
{
	newt_func_t *func = addr;

	return (newt_offset_t) sizeof (newt_func_t) + func->nformal * (newt_offset_t) sizeof (newt_id_t);
}

static void
newt_func_mark(void *addr)
{
	newt_func_t *func = addr;

	if (func->code)
		newt_mark_offset(&newt_code_mem, func->code);
}

static void
newt_func_move(void *addr)
{
	newt_func_t *func = addr;

	if (func->code)
		newt_move_offset(&newt_code_mem, &func->code);
}

const newt_mem_t NEWT_MEM_DECLARE(newt_func_mem) = {
	.size = newt_func_size,
	.mark = newt_func_mark,
	.move = newt_func_move,
	NEWT_MEM_DECLARE_NAME("func")
};
