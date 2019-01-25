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

static newt_frame_t *
newt_frame_realloc(bool globals, newt_offset_t nvariables)
{
	newt_frame_t *frame = newt_alloc(sizeof (newt_frame_t) +
					 nvariables * sizeof (newt_variable_t));

	if (!frame)
		return NULL;

	newt_frame_t *old_frame = newt_pick_frame(globals);
	memcpy(frame, old_frame, sizeof (newt_frame_t));
	frame->nvariables = nvariables;
	return frame;
}

static inline newt_variable_t *
newt_variable_insert(bool globals)
{
	newt_frame_t	*old_frame = newt_pick_frame(globals);
	newt_frame_t	*frame;
	newt_offset_t	nvariables = old_frame->nvariables + 1;

	frame = newt_frame_realloc(globals, old_frame->nvariables + 1);
	if (!frame)
		return NULL;
	old_frame = newt_pick_frame(globals);
	memcpy(frame->variables,
	       old_frame->variables,
	       old_frame->nvariables * sizeof (newt_variable_t));

	if (globals)
		newt_globals = frame;
	else
		newt_frame = frame;
	return &frame->variables[nvariables-1];
}

static inline void
newt_variable_delete(newt_offset_t i)
{
	newt_frame_t	*frame;

	frame = newt_frame_realloc(true, newt_globals->nvariables - 1);
	memcpy(&frame->variables[0],
	       &newt_globals->variables[0],
	       i * sizeof (newt_variable_t));
	memcpy(&frame->variables[i],
	       &newt_globals->variables[i+1],
	       newt_globals->nvariables - i - 1);
}

static newt_variable_t *
newt_variable_lookup(bool globals, newt_id_t id, bool insert)
{
	newt_offset_t	i;
	newt_frame_t	*frame;

	frame = newt_pick_frame(globals);
	for (i = 0; i < frame->nvariables; i++) {
		if (frame->variables[i].id == id)
			return &frame->variables[i];
	}
	if (!insert)
		return NULL;

	newt_variable_t *v = newt_variable_insert(globals);
	if (!v)
		return NULL;

	v->id = id;
	return v;
}

static newt_variable_t *
newt_frame_lookup(newt_id_t id, bool insert)
{
	newt_variable_t	*v = NULL;

	if (newt_frame && (v = newt_variable_lookup(false, id, insert))) {
		if (!newt_is_global(v->value))
			return v;
	}
	if (insert && !newt_globals)
		newt_globals = newt_alloc(sizeof (newt_frame_t));
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

newt_frame_t *
newt_frame_push(newt_code_t *code, newt_offset_t ip, newt_offset_t nformal)
{
	newt_frame_t *f;

	newt_code_stash(code);
	f = newt_alloc(sizeof (newt_frame_t) + nformal * sizeof (newt_variable_t));
	code = newt_code_fetch();
	if (!f)
		return NULL;
	f->nvariables = nformal;
	f->code = newt_pool_offset(code);
	f->ip = ip;
	f->prev = newt_pool_offset(newt_frame);
	newt_frame = f;
	return f;
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

bool
newt_id_del(newt_id_t id)
{
	newt_offset_t i;

	for (i = 0; i < newt_globals->nvariables; i++)
		if (newt_globals->variables[i].id == id) {
			newt_variable_delete(i);
			return true;
		}

	return false;
}

static newt_offset_t
newt_frame_size(void *addr)
{
	newt_frame_t *frame = addr;

	return sizeof (newt_frame_t) + frame->nvariables * sizeof (newt_variable_t);
}

static void
newt_frame_mark(void *addr)
{
	newt_frame_t *f = addr;

	for (;;) {
		debug_memory("\t\tframe mark vars %d code %d prev %d\n",
			     f->variables, f->code, f->prev);
		newt_offset_t i;
		for (i = 0; i < f->nvariables; i++)
			if (!newt_is_global(f->variables[i].value))
				newt_poly_mark(f->variables[i].value);
		if (f->code)
			newt_mark_offset(&newt_code_mem, f->code);
		if (!f->prev || newt_mark_block_offset(&newt_frame_mem, f->prev))
			break;
		f = newt_pool_ref(f->prev);
	}
}

static void
newt_frame_move(void *addr)
{
	newt_frame_t *f = addr;

	for (;;) {
		debug_memory("\t\tframe move vars %d code %d prev %d\n",
			     f->variables, f->code, f->prev);
		newt_offset_t i;
		for (i = 0; i < f->nvariables; i++)
			if (!newt_is_global(f->variables[i].value))
				newt_poly_move(&f->variables[i].value);
		if (f->code)
			newt_move_offset(&newt_code_mem, &f->code);
		if (!f->prev || newt_move_block_offset(&f->prev))
			break;
		f = newt_pool_ref(f->prev);
	}
}

const newt_mem_t NEWT_MEM_DECLARE(newt_frame_mem) = {
	.size = newt_frame_size,
	.mark = newt_frame_mark,
	.move = newt_frame_move,
	NEWT_MEM_DECLARE_NAME("frame")
};
