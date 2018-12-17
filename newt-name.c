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
newt_id_t   newt_id = NEWT_BUILTIN_END;

#define NEWT_BUILTIN_DATA
#include "newt-builtin.h"

static newt_id_t
newt_name_id_builtin(char *name)
{
	ssize_t	l = 0, h = sizeof(newt_builtin_names) - 1;

	while (l <= h) {
		ssize_t m = (l + h) >> 1;

		while (m > l && newt_builtin_names[m-1] != '\0')
			m--;
		if (strcmp((char *) &newt_builtin_names[m+1], name) < 0) {
			l = m + 2;
			while(l < sizeof (newt_builtin_names) - 1 && newt_builtin_names[l-1] != '\0')
				l++;
		} else {
			h = m - 2;
			while(h > 0 && newt_builtin_names[h-1] != '\0')
				h--;
		}
	}
	if (strcmp(name, (char *) &newt_builtin_names[l+1]) == 0)
		return (newt_id_t) newt_builtin_names[l];
	return 0;
}

const char *
newt_name_string_builtin(newt_id_t id)
{
	ssize_t		i;

	i = 0;
	while (i < sizeof(newt_builtin_names)) {
		if (newt_builtin_names[i] == id)
			return (const char *) &newt_builtin_names[i+1];
		i += strlen((const char *) &newt_builtin_names[i+1]) + 2;
	}
	return NULL;
}

newt_id_t
newt_name_id(char *name)
{
	newt_name_t *n;
	newt_id_t id;

	if ((id = newt_name_id_builtin(name)))
		return id;

	for (n = newt_names; n; n = newt_pool_ref(n->next))
		if (!strcmp(n->name, name))
			return n->id;
	n = newt_alloc(sizeof (newt_name_t) + strlen(name) + 1);
	strcpy(n->name, name);
	n->next = newt_pool_offset(newt_names);
	n->id = newt_id++;
	newt_names = n;
	return n->id;
}

const char *
newt_name_string(newt_id_t id)
{
	const char *b;

	if ((b = newt_name_string_builtin(id)))
		return b;

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

const newt_mem_t newt_builtin_mem = {
	.size = newt_null_size,
	.mark = newt_null_mark,
	.move = newt_null_move
};
