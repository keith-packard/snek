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

#ifndef NEWT_BUILTIN_NAMES
#define NEWT_BUILTIN_NAMES(a) (newt_builtin_names[a])
#endif

#ifndef NEWT_BUILTIN_NAMES_CMP
#define NEWT_BUILTIN_NAMES_CMP(a,b) strcmp(a,b)
#endif
#ifndef newt_builtin_names_return
#define newt_builtin_names_return(a) ((const char *) (a))
#endif
#ifndef newt_builtin_names_len
#define newt_builtin_names_len(a) strlen((const char *) (a))
#endif

#if NEWT_BUILTIN_NAMES_SIZE < 128
typedef int8_t newt_bi_index_t;
typedef uint8_t newt_ubi_index_t;
#else
#if NEWT_BUILTIN_NAMES_SIZE < 32768
typedef int16_t newt_bi_index_t;
typedef uint16_t newt_ubi_index_t;
#else
#define int32_t newt_bi_index_t;
typedef uint32_t newt_ubi_index_t;
#endif
#endif

static newt_id_t
newt_name_id_builtin(char *name, bool *keyword)
{
	newt_bi_index_t l = 1, h = sizeof(newt_builtin_names) - 1;

	while (l <= h) {
		newt_bi_index_t m = ((newt_ubi_index_t) (l + h)) >> 1;

		while (NEWT_BUILTIN_NAMES(m-1) != '\0')
			m--;

		if (NEWT_BUILTIN_NAMES_CMP(name, (const char *) &newt_builtin_names[m+1]) > 0) {
			l = m + 2;
			while (NEWT_BUILTIN_NAMES(l-1) != '\0')
				l++;
		} else
			h = m - 2;
	}
	if (NEWT_BUILTIN_NAMES_CMP(name, (const char *) &newt_builtin_names[l+1]) == 0) {
		newt_id_t id = NEWT_BUILTIN_NAMES(l);
		if ((*keyword = ((id & 0x80) != 0)))
			id = (id & ~0x80);
		return id;
	}
	return 0;
}


const char *
newt_name_string_builtin(newt_id_t id)
{
	long		i;

	i = 0;
	while (i < (long) sizeof(newt_builtin_names)) {
		if (newt_builtin_names[i] == id)
			return newt_builtin_names_return(&newt_builtin_names[i+1]);
		i += newt_builtin_names_len((const char *) &newt_builtin_names[i+1]) + 2;
	}
	return NULL;
}

newt_id_t
newt_name_id(char *name, bool *keyword)
{
	newt_name_t *n;
	newt_id_t id;

	if ((id = newt_name_id_builtin(name, keyword)))
		return id;

	*keyword = false;

	for (n = newt_names; n; n = newt_pool_ref(n->next))
		if (!strcmp(n->name, name))
			return n->id;
	n = newt_alloc(sizeof (newt_name_t) + strlen(name) + 1);
	if (!n)
		return NEWT_ID_NONE;
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
	return NULL;
}

static newt_offset_t
newt_name_size(void *addr)
{
	newt_name_t *n = addr;

	return (newt_offset_t) sizeof (newt_name_t) + (newt_offset_t) strlen(n->name) + 1;
}

static void
newt_name_mark(void *addr)
{
	newt_name_t *n = addr;

	for (;;) {
		if (!n->next)
			break;
		n = newt_pool_ref(n->next);
		newt_mark_block_addr(&newt_name_mem, n);
	}
}

void
newt_name_move(void *addr)
{
	newt_name_t *n = addr;

	for (;;) {
		if (!n->next)
			break;
		if (newt_move_block_offset(&n->next))
			break;
		n = newt_pool_ref(n->next);
	}
}

const newt_mem_t NEWT_MEM_DECLARE(newt_name_mem) = {
	.size = newt_name_size,
	.mark = newt_name_mark,
	.move = newt_name_move,
	NEWT_MEM_DECLARE_NAME("name")
};
