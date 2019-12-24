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

static snek_offset_t
snek_list_alloc(snek_offset_t size)
{
	return size + (size >> 3) + (size < 9 ? 3 : 6);
}

snek_poly_t *
snek_list_data(snek_list_t *list)
{
	return snek_pool_addr(list->data);
}

snek_list_t *
snek_list_resize(snek_list_t *list, snek_offset_t size)
{
	if (list->alloc >= size) {
		list->size = size;
		return list;
	}

	snek_offset_t alloc = snek_list_readonly(list) ? size : snek_list_alloc(size);

	snek_stack_push_list(list);
	snek_poly_t *data = snek_alloc(alloc * sizeof (snek_poly_t));
	list = snek_stack_pop_list();

	if (!data)
		return false;
	snek_offset_t to_copy = size;
	if (to_copy > list->size)
		to_copy = list->size;
	memcpy(data, snek_list_data(list), to_copy * sizeof (snek_poly_t));
	list->data = snek_pool_offset(data);
	list->size = size;
	list->alloc = alloc;
	return list;
}

snek_list_t	*snek_empty_tuple;

static snek_list_t *
snek_list_head_alloc(snek_list_type_t type)
{
	snek_list_t *list = snek_alloc(sizeof(snek_list_t));
	if (list)
		snek_list_set_type(list, type);
	return list;
}

snek_list_t *
snek_list_make(snek_offset_t size, snek_list_type_t type)
{
	if (size == 0 && type == snek_list_tuple) {
		if (!snek_empty_tuple)
			return snek_empty_tuple = snek_list_head_alloc(type);
		return snek_empty_tuple;
	}
	snek_list_t *list = snek_list_head_alloc(type);
	if (list)
		list = snek_list_resize(list, size);

	return list;
}

snek_list_t *
snek_list_append(snek_list_t *list, snek_list_t *append)
{
	snek_offset_t oldsize = list->size;
	snek_offset_t append_size = append->size;

	if (append == list)
		append = NULL;
	else
		snek_stack_push_list(append);
	list = snek_list_resize(list, list->size + append_size);
	if (append)
		append = snek_stack_pop_list();
	else
		append = list;

	if (list)
		memcpy(snek_list_data(list) + oldsize,
		       snek_list_data(append),
		       append_size * sizeof(snek_poly_t));
	return list;
}

snek_list_t *
snek_list_plus(snek_list_t *a, snek_list_t *b)
{
	snek_stack_push_list(a);
	snek_stack_push_list(b);
	snek_list_t *n = snek_list_make(a->size + b->size, snek_list_type(a));
	b = snek_stack_pop_list();
	a = snek_stack_pop_list();
	if (!n)
		return NULL;
	snek_poly_t *ndata = snek_list_data(n);
	snek_poly_t *adata = snek_list_data(a);
	memcpy(ndata, adata, a->size * sizeof(snek_poly_t));
	snek_poly_t *bdata = snek_list_data(b);
	memcpy(ndata + a->size, bdata, b->size * sizeof(snek_poly_t));
	return n;
}

snek_list_t *
snek_list_times(snek_list_t *a, snek_soffset_t count)
{
	if (count < 0)
		count = 0;
	snek_stack_push_list(a);
	snek_offset_t size = a->size;
	snek_list_t *n = snek_list_make(size * count, snek_list_type(a));
	a = snek_stack_pop_list();
	if (!n)
		return NULL;
	snek_poly_t *src = snek_list_data(a);
	snek_poly_t *dst = snek_list_data(n);
	while (count--) {
		memcpy(dst, src, size * sizeof (snek_poly_t));
		dst += size;
	}
	return n;
}

static bool
snek_mutable(snek_poly_t p)
{
	if (snek_poly_type(p) != snek_list)
		return false;
	snek_list_t *l = snek_poly_to_list(p);
	if (!snek_list_readonly(l))
		return true;
	snek_offset_t o;
	snek_poly_t *ldata = snek_list_data(l);
	for (o = 0; o < l->size; o++)
		if (snek_mutable(ldata[o]))
			return true;
	return false;
}

static snek_poly_t *
_snek_list_ref(snek_list_t *list, snek_poly_t p, bool report_error, bool add)
{
	snek_offset_t o;
	snek_poly_t *data = snek_list_data(list);

	if (snek_list_type(list) == snek_list_dict) {
		snek_offset_t l = 0, r = list->size;
		while (l < r) {
			o = ((l + r) >> 1) & ~1;
			snek_poly_t i = data[o];
			if (snek_poly_cmp(i, p, false) < 0)
				l = o + 2;
			else
				r = o;
		}
		o = l;
		if (o >= list->size || snek_poly_cmp(p, data[o], false) != 0) {
			if (!add)
				goto fail;
			if (snek_mutable(p))
				goto fail;
			snek_stack_push(p);
			list = snek_list_resize(list, list->size + 2);
			p = snek_stack_pop();
			if (!list)
				return NULL;
			data = snek_list_data(list);
			memmove(data + o + 2, data + o, (list->size - o - 2) * sizeof (snek_poly_t));
			data[o] = p;
		}
		o++;
	} else {
		snek_soffset_t so = snek_poly_get_soffset(p);
		o = so;
		if (so < 0)
			o = list->size - (snek_offset_t) (-so);
		if (list->size <= o)
			goto fail;
	}
	return &data[o];
fail:
	if (report_error)
		snek_error_value(p);
	return NULL;
}

snek_poly_t *
snek_list_ref(snek_list_t *list, snek_poly_t p, bool report_error)
{
	return _snek_list_ref(list, p, report_error, true);
}

snek_poly_t
snek_list_get(snek_list_t *list, snek_poly_t p, bool report_error)
{
	snek_poly_t *r = _snek_list_ref(list, p, report_error, false);
	if (r)
		return *r;
	return SNEK_NULL;
}

void
snek_list_del(snek_poly_t lp, snek_poly_t p)
{
	snek_list_t *list = snek_poly_to_list(lp);
	snek_poly_t *r = snek_list_ref(list, p, true);
	if (!r)
		return;
	snek_offset_t	num = 1;
	switch (snek_list_type(list)) {
	case snek_list_dict:
		r--;
		num = 2;
		break;
	case snek_list_tuple:
		snek_error_value(lp);
		return;
	default:
		break;
	}
	snek_offset_t remain = snek_list_data(list) + list->size - r;
	memmove(r, r + num, (remain - num) * sizeof (snek_poly_t));
	list->size -= num;
}

int8_t
snek_list_cmp(snek_list_t *a, snek_list_t *b)
{
	int8_t diff = snek_list_type(a) - snek_list_type(b);
	if (diff)
		return diff;
	snek_poly_t *adata = snek_list_data(a);
	snek_poly_t *bdata = snek_list_data(b);

	snek_offset_t o;
	for (o = 0; o < a->size; o++) {
		if (o >= b->size)
			return 1;

		diff = snek_poly_cmp(adata[o], bdata[o], false);
		if (diff)
			return diff;
	}
	return b->size > o;
}

snek_poly_t
snek_list_imm(snek_offset_t size, snek_list_type_t type)
{
	snek_list_t	*list = snek_list_make(size, type);

	if (!list) {
		snek_stack_drop(size);
		return SNEK_NULL;
	}

	snek_poly_t	*data = snek_list_data(list);
	if (type == snek_list_dict) {
		list->size = 0;
		snek_offset_t s = size;
		while (s) {
			s -= 2;
			snek_poly_t key = snek_stack_pick(s+1);
			snek_poly_t value = snek_stack_pick(s);
			snek_poly_t *ref = _snek_list_ref(list, key, false, true);
			if (ref)
				*ref = value;
		}
		snek_stack_drop(size);
	} else {
		while (size--)
			data[size] = snek_stack_pop();
	}
	return snek_list_to_poly(list);
}

#ifdef SNEK_LIST_BUILD
snek_poly_t
snek_list_build(snek_list_type_t type, snek_offset_t size, ...)
{
	va_list args;
	va_start(args, size);
	snek_offset_t s = size;
	while (s--)
		snek_stack_push(va_arg(args, snek_poly_t));
	return snek_list_imm(size, type);
}
#endif

snek_list_t *
snek_list_slice(snek_list_t *list, snek_slice_t *slice)
{
	bool readonly = snek_list_readonly(list);
	if (readonly && slice->identity)
	    return list;

	snek_stack_push_list(list);
	snek_list_t *n = snek_list_make(slice->count, readonly);
	list = snek_stack_pop_list();
	if (!n)
		return NULL;
	snek_offset_t i = 0;
	snek_poly_t *data = snek_list_data(list);
	snek_poly_t *ndata = snek_list_data(n);
	for (; snek_slice_test(slice); snek_slice_step(slice))
		ndata[i++] = data[slice->pos];
	return n;
}

void
snek_stack_push_list(snek_list_t *l)
{
	snek_stack_push(snek_list_to_poly(l));
}

snek_list_t *
snek_stack_pop_list(void)
{
	return snek_poly_to_list(snek_stack_pop());
}

snek_offset_t
snek_list_size(void *addr)
{
	(void) addr;
	return sizeof (snek_list_t);
}

void
snek_list_mark(void *addr)
{
	snek_list_t *list = addr;
	debug_memory("\t\tmark list size %d alloc %d data %d\n", list->size, list->alloc, list->data);
	if (list->alloc) {
		snek_poly_t *data = snek_list_data(list);
		snek_mark_blob(data, list->alloc * sizeof (snek_poly_t));
		for (snek_offset_t i = 0; i < list->size; i++)
			snek_poly_mark_ref(&data[i]);
	}
}

void
snek_list_move(void *addr)
{
	snek_list_t *list = addr;
	debug_memory("\t\tmove list size %d alloc %d data %d\n", list->size, list->alloc, list->data);
	if (list->alloc) {
		snek_move_block_offset(&list->data);
		snek_poly_t *data = snek_list_data(list);
		for (snek_offset_t i = 0; i < list->size; i++)
			snek_poly_move(&data[i]);
	}
}
