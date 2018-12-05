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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef union {
	uint32_t    u;
	float       f;
} newt_poly_t;

typedef enum {
	newt_op_nop,
	newt_op_num,
	newt_op_name,

	newt_op_eq,
	newt_op_ne,
	newt_op_gt,
	newt_op_lt,
	newt_op_ge,
	newt_op_le,

	newt_op_plus,
	newt_op_minus,
	newt_op_times,
	newt_op_divide,
	newt_op_mod,

	newt_op_assign,

} __attribute__((packed)) newt_op_t;

typedef enum {
	newt_list = 0,
	newt_string = 1,
	newt_float = 4,
} __attribute__((packed)) newt_type_t;

typedef struct newt_mem {
	int	(*size)(void *addr);
	void	(*mark)(void *addr);
	void	(*move)(void *addr);
	char	name[];
} newt_mem_t;

typedef struct newt_list {
	uint16_t	size;
	uint16_t	alloc;
	newt_poly_t	*data;
} newt_list_t;

#define NEWT_NAN	0xffffffffu
#define NEWT_NULL	0xfffffffeu

#include "newt-gram.h"

/* newt-main.c */
int
yylex(void);

void
yyerror (char *msg);

int
yywrap(void);

/* newt-list.c */

extern const newt_mem_t newt_list_mem;

/* newt-memory.c */

#define NEWT_POOL		8192
#define NEWT_POOL_EXTRA		0
#define NEWT_ALLOC_SHIFT	2
#define NEWT_ALLOC_ROUND	(1 << NEWT_ALLOC_SHIFT)

#if NEWT_POOL <= 65536
typedef uint16_t	newt_offset_t;
#else
typedef uint32_t	newt_offset_t;
#endif

#define NEWT_COLLECT_FULL		0
#define NEWT_COLLECT_INCREMENTAL	1

int
newt_poly_mark(newt_poly_t p, uint8_t do_note_cons);

newt_offset_t
newt_collect(uint8_t style);

int
newt_mark_memory(const struct newt_mem *type, void *addr);

int
newt_move_memory(const struct newt_mem *type, void **ref);

int
newt_poly_move(newt_offset_t *ref, uint8_t do_note_cons);

int
newt_marked(void *addr);

void *
newt_alloc(newt_offset_t size);

void
newt_poly_stash(newt_offset_t p);

newt_offset_t
newt_poly_fetch(void);

int
newt_print_mark_addr(void *addr);

void
newt_print_clear_addr(void *addr);

void
newt_print_start(void);

int
newt_print_stop(void);

/* newt-name.c */
char *
newt_name_find(char *name);

/* newt-poly.c */

void *
newt_ref(newt_poly_t poly);

newt_poly_t
newt_poly(const void *addr, newt_type_t type);

static inline bool
newt_is_float(newt_poly_t v)
{
	if ((v.u & 0xff000000) != 0xff000000 || v.u == NEWT_NAN)
		return true;
	return false;
}

static inline newt_poly_t
newt_float_to_value(float f)
{
	if (isnanf(f))
		return (newt_poly_t) { .u = NEWT_NAN };
	return (newt_poly_t) { .f = f };
}

static inline newt_poly_t
newt_uint_to_value(uint32_t u)
{
	return (newt_poly_t) { .u = 0xff000000 | u };
}

static inline uint32_t
newt_poly_to_uint(newt_poly_t v)
{
	return v.u;
}

static inline float
newt_poly_to_float(newt_poly_t v)
{
	return v.f;
}

static inline newt_type_t
newt_type(newt_poly_t v)
{
	return newt_is_float(v) ? newt_float : (v.u & 3);
}

static inline newt_poly_t
newt_list_to_poly(newt_list_t *list)
{
	return newt_poly(list, newt_list);
}

static inline newt_list_t *
newt_poly_to_list(newt_poly_t poly)
{
	return newt_ref(poly);
}
