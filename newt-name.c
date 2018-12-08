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

newt_name_t *newt_names;
newt_id_t   newt_id;

newt_id_t
newt_name_id(char *name)
{
	newt_name_t *n;
	for (n = newt_names; n; n = newt_pool_ref(n->next))
		if (!strcmp(n->name, name))
			return n->id;
	n = newt_alloc(sizeof (newt_name_t) + strlen(name) + 1);
	strcpy(n->name, name);
	n->next = newt_pool_offset(newt_names);
	n->id = ++newt_id;
	newt_names = n;
	return n->id;
}

char *
newt_name_string(newt_id_t id)
{
	newt_name_t *n;
	for (n = newt_names; n; n = newt_pool_ref(n->next))
		if (n->id == id)
			return n->name;
	return "<unknown>";
}

static int
newt_name_size(void *addr)
{
	newt_name_t *n = addr;

	return sizeof (newt_name_t) + strlen(n->name) + 1;
}

static void
newt_name_mark(void *addr)
{
	newt_name_t *n = addr;

	for (;;) {
		n = newt_pool_ref(n->next);
		if (!n)
			break;
		newt_mark_memory(&newt_name_mem, n);
	}
}

void
newt_name_move(void *addr)
{
	newt_name_t *n = addr;

	for (;;) {
		newt_name_t *next = newt_pool_ref(n->next);
		if (!next)
			break;
		int ret = newt_move_memory((void **) &next);
		if (next != newt_pool_ref(n->next))
			n->next = newt_pool_offset(next);
		if (ret)
			break;
		n = next;
	}
}

const newt_mem_t newt_name_mem = {
	.size = newt_name_size,
	.mark = newt_name_mark,
	.move = newt_name_move,
	.name = "name"
};
