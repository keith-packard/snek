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

snek_name_t *snek_names;
snek_id_t   snek_id = SNEK_BUILTIN_END;

#define SNEK_BUILTIN_DATA
#include "snek-builtin.h"

#ifndef SNEK_BUILTIN_NAMES
#define SNEK_BUILTIN_NAMES(a) (snek_builtin_names[a])
#endif

#ifndef snek_builtin_names_return
#define snek_builtin_names_return(a) ((const char *) (a))
#endif
#ifndef snek_builtin_names_len
#define snek_builtin_names_len(a) strlen((const char *) (a))
#endif

#if SNEK_BUILTIN_NAMES_SIZE < 256
typedef uint8_t snek_bi_index_t;
#else
#if SNEK_BUILTIN_NAMES_SIZE < 65536
typedef uint16_t snek_bi_index_t;
#else
tyepdef uint32_t snek_bi_index_t;
#endif
#endif

#define SNEK_BUILTIN_ID(i)	SNEK_BUILTIN_NAMES(i)

static snek_id_t
snek_name_id_builtin(char *name, bool *keyword)
{
	snek_bi_index_t i;
	snek_bi_index_t k = 0;
	snek_id_t id = 1;

	for (i = 0;
	     i < sizeof (snek_builtin_names);
	     i += snek_builtin_names_len(&snek_builtin_names[i+k]) + 1 + k)
	{
		if (snek_const_strcmp(name, &snek_builtin_names[i+k]) == 0) {
			if (id >= SNEK_BUILTIN_END) {
				id = SNEK_BUILTIN_ID(i);
				*keyword = true;
			} else {
				*keyword = false;
			}
			return id;
		}
		if (++id == SNEK_BUILTIN_END)
			k = 1;
	}
	return 0;
}

static const char *
snek_name_string_builtin(snek_id_t id)
{
	if (id >= SNEK_BUILTIN_END)
		return NULL;

	snek_bi_index_t i;

	for (i = 0;
	     --id;
	     i += snek_builtin_names_len(&snek_builtin_names[i]) + 1)
		;
	return snek_builtin_names_return(&snek_builtin_names[i]);
}

snek_id_t
snek_name_id(char *name, bool *keyword)
{
	snek_name_t *n;
	snek_id_t id;

	if ((id = snek_name_id_builtin(name, keyword)))
		return id;

	*keyword = false;

	id = snek_id;
	for (n = snek_names; n; n = snek_pool_addr(n->next)) {
		if (!strcmp(n->name, name))
			return id;
		id--;
	}
	n = snek_alloc(sizeof (snek_name_t) + strlen(name) + 1);
	if (!n)
		return SNEK_ID_NONE;
	strcpy(n->name, name);
	n->next = snek_pool_offset(snek_names);
	snek_id++;
	snek_names = n;
	return snek_id;
}

const char *
snek_name_string(snek_id_t match_id)
{
	const char *b;

	if ((b = snek_name_string_builtin(match_id)))
		return b;

	snek_name_t *n;

	snek_id_t id = snek_id;

	for (n = snek_names; n; n = snek_pool_addr(n->next)) {
		if (id == match_id)
			return n->name;
		id--;
	}
	return NULL;
}

static snek_offset_t
snek_name_size(void *addr)
{
	snek_name_t *n = addr;

	return (snek_offset_t) sizeof (snek_name_t) + (snek_offset_t) strlen(n->name) + 1;
}

static void
snek_name_mark(void *addr)
{
	snek_name_t *n = addr;

	for (;;) {
		n = snek_pool_addr(n->next);
		if (!n)
			break;
		snek_mark_block_addr(&snek_name_mem, n);
	}
}

static void
snek_name_move(void *addr)
{
	snek_name_t *n = addr;

	for (;;) {
		if (snek_move_block_offset(&n->next))
			break;
		n = snek_pool_addr(n->next);
	}
}

CONST snek_mem_t snek_name_mem = {
	.size = snek_name_size,
	.mark = snek_name_mark,
	.move = snek_name_move,
	SNEK_MEM_DECLARE_NAME("name")
};
