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

static inline newt_offset_t
newt_list_alloc(newt_offset_t size)
{
	return size + (size >> 3) + (size < 9 ? 3 : 6);
}

static newt_list_t *
newt_list_resize(newt_list_t *list, newt_offset_t size)
{
	newt_offset_t alloc = list->readonly ? size : newt_list_alloc(size);

	newt_poly_stash(newt_list_to_poly(list));
	newt_poly_t *data = newt_alloc(alloc * sizeof (newt_poly_t));
	list = newt_poly_to_list(newt_poly_fetch());

	if (!data)
		return false;
	if (list->data) {
		newt_offset_t to_copy = size;
		if (to_copy > list->size)
			to_copy = list->size;
		memcpy(data, newt_pool_ref(list->data), to_copy * sizeof (newt_poly_t));
	}
	list->data = newt_pool_offset(data);
	list->size = size;
	list->alloc = alloc;
	return list;
}

newt_list_t *
newt_list_make(newt_offset_t size, bool readonly)
{
	newt_list_t	*list;

	list = newt_alloc(sizeof (newt_list_t));
	if (!list)
		return NULL;

	list->readonly = readonly;

	list = newt_list_resize(list, size);
	if (!list)
		return NULL;

	return list;
}

newt_list_t *
newt_list_append(newt_list_t *list, newt_list_t *append)
{
	newt_offset_t oldsize = list->size;

	if (list->readonly)
		return NULL;

	newt_poly_stash(newt_list_to_poly(append));
	list = newt_list_resize(list, list->size + append->size);
	append = newt_poly_to_list(newt_poly_fetch());

	if (list)
		memcpy((newt_poly_t *) newt_pool_ref(list->data) + oldsize,
		       newt_pool_ref(append->data),
		       append->size * sizeof(newt_poly_t));
	return list;
}

newt_list_t *
newt_list_plus(newt_list_t *a, newt_list_t *b)
{
	newt_poly_stash(newt_list_to_poly(a));
	newt_poly_stash(newt_list_to_poly(b));
	newt_list_t *n = newt_list_make(a->size + b->size, a->readonly);
	b = newt_poly_to_list(newt_poly_fetch());
	a = newt_poly_to_list(newt_poly_fetch());
	if (!n)
		return NULL;
	memcpy(newt_pool_ref(n->data),
	       newt_pool_ref(a->data),
	       a->size * sizeof(newt_poly_t));
	memcpy((newt_poly_t *) newt_pool_ref(n->data) + a->size,
	       newt_pool_ref(b->data),
	       b->size * sizeof(newt_poly_t));
	return n;
}

bool
newt_list_equal(newt_list_t *a, newt_list_t *b)
{
	if (a->size != b->size)
		return false;
	newt_poly_t *adata = newt_pool_ref(a->data);
	newt_poly_t *bdata = newt_pool_ref(b->data);
	for (newt_offset_t o = 0; o < a->size; o++)
		if (!newt_poly_equal(adata[o], bdata[o]))
			return false;
	return true;
}

newt_list_t *
newt_list_imm(newt_offset_t size, bool readonly)
{
	newt_list_t	*list = newt_list_make(size, readonly);

	if (!list)
		return NULL;

	newt_poly_t	*data = newt_pool_ref(list->data);
	while (size--)
		data[size] = newt_stack_pop();
	return list;
}

newt_list_t *
newt_list_slice(newt_list_t *list, newt_slice_t *slice)
{
	if (list->readonly && newt_slice_identity(slice))
	    return list;

	newt_poly_stash(newt_list_to_poly(list));
	newt_list_t *n = newt_list_make(slice->count, list->readonly);
	list = newt_poly_to_list(newt_poly_fetch());
	if (!n)
		return NULL;
	newt_offset_t i = 0;
	newt_poly_t *data = newt_pool_ref(list->data);
	newt_poly_t *ndata = newt_pool_ref(n->data);
	for (newt_slice_start(slice); newt_slice_test(slice); newt_slice_step(slice))
		ndata[i++] = data[slice->pos];
	return n;
}

static int
newt_list_size(void *addr)
{
	(void) addr;
	return sizeof (newt_list_t);
}

static void
newt_list_mark(void *addr)
{
	newt_list_t *list = addr;
	newt_poly_t *data = newt_pool_ref(list->data);
	newt_mark_blob(data, list->alloc * sizeof (newt_poly_t));
	for (newt_offset_t i = 0; i < list->size; i++)
		newt_poly_mark(data[i], 1);
}

static void
newt_list_move(void *addr)
{
	newt_list_t *list = addr;
	newt_move_offset(&list->data);
	newt_poly_t *data = newt_pool_ref(list->data);
	for (newt_offset_t i = 0; i < list->size; i++)
		newt_poly_move(&data[i], 1);
}

const newt_mem_t newt_list_mem = {
	.size = newt_list_size,
	.mark = newt_list_mark,
	.move = newt_list_move,
	.name = "list",
};
