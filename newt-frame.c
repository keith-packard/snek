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

newt_frame_t	*newt_globals;
newt_frame_t	*newt_frame;

static inline newt_frame_t *newt_pick_frame(bool globals)
{
	if (globals)
		return newt_globals;
	return newt_frame;
}

static newt_variable_t *
newt_variable_lookup(bool globals, newt_id_t id, bool insert)
{
	newt_variable_t	*v;

	for (v = newt_pool_ref(newt_pick_frame(globals)->variables); v; v = newt_pool_ref(v->next)) {
		if (v->id == id)
			return v;
	}
	if (!insert)
		return NULL;

	v = newt_alloc(sizeof (newt_variable_t));
	if (!v)
		return NULL;

	v->id = id;
	v->next = newt_pick_frame(globals)->variables;
	newt_pick_frame(globals)->variables = newt_pool_offset(v);
	return v;
}

static void
newt_frame_ensure_globals(void)
{
	if (!newt_globals)
		newt_globals = newt_alloc(sizeof (newt_frame_t));
}

newt_variable_t *
newt_frame_lookup(newt_id_t id, bool insert)
{
	newt_variable_t	*v = NULL;

	if (newt_frame && (v = newt_variable_lookup(false, id, insert))) {
		if (!newt_is_global(v->value))
			return v;
	}
	if (insert)
		newt_frame_ensure_globals();
	if (newt_globals && (v = newt_variable_lookup(true, id, insert)))
		return v;
	return v;
}

bool
newt_frame_mark_global(newt_id_t id)
{
	if (newt_frame) {
		newt_variable_t *v;

		v = newt_variable_lookup(false, id, true);
		if (!v)
			return false;
		v->value = NEWT_GLOBAL;
	}
	return true;
}

bool
newt_frame_push(newt_code_t *code, newt_offset_t ip)
{
	newt_frame_t *f;

	newt_code_stash(code);
	f = newt_alloc(sizeof (newt_frame_t));
	code = newt_code_fetch();
	if (!f)
		return false;
	f->code = newt_pool_offset(code);
	f->ip = ip;
	f->prev = newt_pool_offset(newt_frame);
	newt_frame = f;
	return true;
}

newt_code_t *
newt_frame_pop(newt_offset_t *ip_p)
{
	if (!newt_frame)
		return NULL;

	newt_code_t	*code = newt_pool_ref(newt_frame->code);
	newt_offset_t	ip = newt_frame->ip;

	newt_frame = newt_pool_ref(newt_frame->prev);

	*ip_p = ip;
	return code;
}

newt_poly_t *
newt_id_ref(newt_id_t id, bool insert)
{
	newt_variable_t *v = newt_frame_lookup(id, insert);
	if (!v)
		return NULL;
	return &v->value;
}

static int
newt_variable_size(void *addr)
{
	(void) addr;
	return sizeof (newt_variable_t);
}

static void
newt_variable_mark(void *addr)
{
	newt_variable_t *v = addr;

	for (;;) {
		newt_variable_t *next = newt_pool_ref(v->next);

		newt_poly_mark(v->value, 1);
		if (!next)
			break;
		newt_mark_memory(&newt_variable_mem, next);
		v = next;
	}
}

static void
newt_variable_move(void *addr)
{
	newt_variable_t *v = addr;

	for (;;) {
		int ret;

		(void) newt_poly_move(&v->value, true);

		newt_variable_t *next = newt_pool_ref(v->next);

		ret = newt_move_memory((void **) &next);
		if (next != newt_pool_ref(v->next))
			v->next = newt_pool_offset(next);
		if (ret)
			break;

		v = next;
	}
}

const newt_mem_t newt_variable_mem = {
	.size = newt_variable_size,
	.mark = newt_variable_mark,
	.move = newt_variable_move,
	.name = "variable"
};

static int
newt_frame_size(void *addr)
{
	(void) addr;
	return sizeof (newt_frame_t);
}

static void
newt_frame_mark(void *addr)
{
	newt_frame_t *f = addr;

	for (;;) {
		newt_mark(&newt_variable_mem, newt_pool_ref(f->variables));
		if (f->code)
			newt_mark(&newt_code_mem, newt_pool_ref(f->code));
		newt_frame_t *prev = newt_pool_ref(f->prev);
		if (!prev)
			break;
		newt_mark_memory(&newt_frame_mem, prev);
		f = prev;
	}
}

static void
newt_frame_move(void *addr)
{
	newt_frame_t *f = addr;

	for (;;) {
		newt_variable_t *v = newt_pool_ref(f->variables);
		if (v) {
			newt_move(&newt_variable_mem, (void **) &v);
			if (v != newt_pool_ref(f->variables))
				f->variables = newt_pool_offset(v);
		}
		newt_code_t *c = newt_pool_ref(f->code);
		if (c) {
			newt_move(&newt_code_mem, (void **) &c);
			if (c != newt_pool_ref(f->code))
				f->code = newt_pool_offset(c);
		}
		newt_frame_t *prev = newt_pool_ref(f->prev);
		if (!prev)
			break;
		int ret = newt_move_memory((void **) &prev);
		if (prev != newt_pool_ref(f->prev))
			f->prev = newt_pool_offset(prev);
		if (ret)
			break;
		f = prev;
	}
}

const newt_mem_t newt_frame_mem = {
	.size = newt_frame_size,
	.mark = newt_frame_mark,
	.move = newt_frame_move,
	.name = "frame"
};
