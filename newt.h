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
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "newt-builtin.h"

#ifndef NEWT_DEBUG
#define NEWT_DEBUG	1
#endif

// #define DEBUG_MEMORY

#ifdef DEBUG_MEMORY
#define debug_memory(fmt, args...) printf(fmt, ## args)
#else
#define debug_memory(fmt, args...)
#endif

#ifndef NEWT_POOL
#define NEWT_POOL		(4 * 1024)
#endif
#define NEWT_POOL_EXTRA		0
#define NEWT_ALLOC_SHIFT	2
#define NEWT_ALLOC_ROUND	(1 << NEWT_ALLOC_SHIFT)
#define NEWT_OFFSET_MASK	0x00fffffc

#if NEWT_POOL <= 65536
typedef uint16_t	newt_offset_t;
typedef int16_t		newt_soffset_t;
#define NEWT_OFFSET_NONE	0xffffu
#define NEWT_SOFFSET_NONE	0x7fff
#else
typedef uint32_t	newt_offset_t;
typedef int32_t		newt_soffset_t;
#define NEWT_OFFSET_NONE	0xffffffffu
#define NEWT_SOFFSET_NONE	0x7fffffff
#endif

typedef newt_offset_t newt_id_t;

#define NEWT_ID_NONE		0

typedef union {
	uint32_t    u;
	float       f;
} newt_poly_t;

typedef enum {
	newt_op_plus = 0,
	newt_op_minus = 1,
	newt_op_times = 2,
	newt_op_divide = 3,
	newt_op_div = 4,
	newt_op_mod = 5,
	newt_op_pow = 6,
	newt_op_land = 7,
	newt_op_lor = 8,
	newt_op_lxor = 9,
	newt_op_lshift = 10,
	newt_op_rshift = 11,

	newt_op_assign_plus = 12,
	newt_op_assign_minus = 13,
	newt_op_assign_times = 14,
	newt_op_assign_divide = 15,
	newt_op_assign_div = 16,
	newt_op_assign_mod = 17,
	newt_op_assign_pow = 18,
	newt_op_assign_land = 19,
	newt_op_assign_lor = 20,
	newt_op_assign_lxor = 21,
	newt_op_assign_lshift = 22,
	newt_op_assign_rshift = 23,

	newt_op_num,
	newt_op_int,
	newt_op_string,
	newt_op_list,
	newt_op_tuple,
	newt_op_id,

	newt_op_not,

	newt_op_eq,
	newt_op_ne,
	newt_op_gt,
	newt_op_lt,
	newt_op_ge,
	newt_op_le,

	newt_op_is,
	newt_op_is_not,
	newt_op_in,
	newt_op_not_in,

	newt_op_uminus,
	newt_op_lnot,

	newt_op_call,

	newt_op_array,
	newt_op_slice,

	newt_op_assign,

	newt_op_global,

	newt_op_branch,
	newt_op_branch_true,
	newt_op_branch_false,
	newt_op_forward,
	newt_op_range_start,
	newt_op_range_step,
	newt_op_in_start,
	newt_op_in_step,

	newt_op_line,

	newt_op_push = 0x80,
} __attribute__((packed)) newt_op_t;

typedef enum {
	newt_forward_return,
	newt_forward_break,
	newt_forward_continue,
	newt_forward_if,
} __attribute__((packed)) newt_forward_t;

typedef enum {
	newt_list = 0,
	newt_string = 1,
	newt_func = 2,
	newt_builtin = 3,
	newt_float = 4,
} __attribute__((packed)) newt_type_t;

typedef struct newt_mem {
	newt_offset_t	(*size)(void *addr);
	void		(*mark)(void *addr);
	void		(*move)(void *addr);
#ifdef NEWT_MEM_INCLUDE_NAME
	char		name[];
#endif
} newt_mem_t;

#ifndef NEWT_MEM_DECLARE
#define NEWT_MEM_DECLARE(n) n
#define NEWT_MEM_SIZE(m)	((m)->size)
#define NEWT_MEM_MARK(m)	((m)->mark)
#define NEWT_MEM_MOVE(m)	((m)->move)
#endif

#ifdef NEWT_MEM_INCLUDE_NAME
#define NEWT_MEM_DECLARE_NAME(_name)	.name = _name,
#else
#define NEWT_MEM_DECLARE_NAME(_name)
#endif

typedef struct newt_list {
	newt_offset_t	size;
	newt_offset_t	alloc;
	newt_offset_t	note_next_and_readonly;
	newt_offset_t	data;
} newt_list_t;

typedef struct newt_code {
	newt_offset_t	size;
	uint8_t		code[0];
} newt_code_t;

typedef struct newt_range {
	newt_offset_t	prev;
	newt_id_t	id;
	float		current;
	float		limit;
	float		step;
} newt_range_t;

typedef struct newt_in {
	newt_offset_t	prev;
	newt_id_t	id;
	newt_poly_t	array;
	newt_offset_t	i;
} newt_in_t;

typedef struct newt_func {
	newt_soffset_t	nformal;
	newt_offset_t	code;
	newt_id_t	formals[0];
} newt_func_t;

#define NEWT_FUNC_VARARGS	NEWT_SOFFSET_NONE

typedef struct newt_name {
	newt_offset_t	next;
	newt_id_t	id;
	char		name[0];
} newt_name_t;

typedef struct newt_variable {
	newt_id_t	id;
	newt_poly_t	value;
} newt_variable_t;

typedef struct newt_frame {
	newt_offset_t	prev;
	newt_offset_t	code;
	newt_offset_t	ip;
	newt_offset_t	nvariables;
	newt_variable_t	variables[0];
} newt_frame_t;


typedef struct newt_slice {
	/* provided parameters */
	int32_t		start;
	int32_t		end;
	int32_t		stride;

	/* provided length of object */
	newt_offset_t	len;

	/* computed number of outputs */
	newt_offset_t	count;

	/* computed position of current object */
	newt_offset_t	pos;
} newt_slice_t;

#define NEWT_SLICE_DEFAULT	0x7fffffff	/* empty value provided [1:] */

typedef struct newt_builtin {
	int8_t nformal;
	union {
		newt_poly_t	(*funcv)(int nactuals, ...);
		newt_poly_t	(*func0)(void);
		newt_poly_t	(*func1)(newt_poly_t a0);
		newt_poly_t	(*func2)(newt_poly_t a0, newt_poly_t a1);
		newt_poly_t	(*func3)(newt_poly_t a0, newt_poly_t a1, newt_poly_t a2);
		newt_poly_t	(*func4)(newt_poly_t a0, newt_poly_t a1, newt_poly_t a2, newt_poly_t a3);
	};
} newt_builtin_t;

extern const newt_builtin_t newt_builtins[];

#define NEWT_BUILTIN_VARARGS	-1

#define NEWT_NAN_U	0x7fffffffu
#define NEWT_NAN	((newt_poly_t) { .u = NEWT_NAN_U })
#define NEWT_NULL_U	0xffffffffu
#define NEWT_NULL	((newt_poly_t) { .u = NEWT_NULL_U })
#define NEWT_GLOBAL_U	0xfffffffeu
#define NEWT_GLOBAL	((newt_poly_t) { .u = NEWT_GLOBAL_U })
#define NEWT_ZERO	((newt_poly_t) { .f = 0.0f })
#define NEWT_ONE	((newt_poly_t) { .f = 1.0f })

#ifndef NEWT_STACK
#define NEWT_STACK	256
#endif
extern newt_poly_t	newt_stack[NEWT_STACK];
extern newt_offset_t	newt_stackp;

static inline bool
newt_is_nan(newt_poly_t p)
{
	return p.u == NEWT_NAN_U;
}

static inline bool
newt_is_null(newt_poly_t p)
{
	return p.u == NEWT_NULL_U;
}

static inline bool
newt_is_global(newt_poly_t p)
{
	return p.u == NEWT_GLOBAL_U;
}


#ifdef NEWT_DYNAMIC
extern uint8_t *newt_pool  __attribute__((aligned(NEWT_ALLOC_ROUND)));
extern uint32_t	newt_pool_size;
#else
extern uint8_t	newt_pool[NEWT_POOL + NEWT_POOL_EXTRA] __attribute__((aligned(NEWT_ALLOC_ROUND)));
#endif

#include "newt-gram.h"

typedef union {
	bool		bools;
	int		ints;
	newt_op_t	op;
	newt_offset_t	offset;
	newt_id_t	id;
	float		number;
	char		*string;
} newt_token_val_t;

extern newt_token_val_t	newt_token_val;

/* newt-builtin.c */

/* newt-code.c */

extern const char * const newt_op_names[];

extern uint8_t		*newt_compile;
extern newt_offset_t	newt_compile_size;
extern newt_offset_t	newt_compile_prev, newt_compile_prev_prev;

#define NEWT_OP_SLICE_START	1
#define NEWT_OP_SLICE_END	2
#define NEWT_OP_SLICE_STRIDE	4

void
newt_code_add_op(newt_op_t op);

void
newt_code_add_op_id(newt_op_t op, newt_id_t id);

void
newt_code_add_op_array(newt_op_t op);

void
newt_code_add_number(float number);

void
newt_code_add_string(char *string);

void
newt_code_add_op_offset(newt_op_t op, newt_offset_t offset);

void
newt_code_add_op_branch(newt_op_t op);

void
newt_code_add_forward(newt_forward_t forward);

void
newt_code_patch_forward(newt_offset_t start, newt_forward_t forward, newt_offset_t target);

void
newt_code_add_slice(bool has_start, bool has_end, bool has_stride);

void
newt_code_add_range_start(newt_id_t id, newt_offset_t nactual);

void
newt_code_patch_branch(newt_offset_t branch, newt_offset_t target);

newt_code_t *
newt_code_finish(void);

newt_offset_t
newt_code_line(newt_code_t *code);

void
newt_run_mark(void);

void
newt_run_move(void);

newt_poly_t
newt_code_run(newt_code_t *code);

newt_poly_t
newt_accumulator(void);

extern const newt_mem_t newt_code_mem;
extern const newt_mem_t newt_compile_mem;

/* newt-error.c */

#ifndef newt_error_name
#define newt_error_name newt_error
#endif

void
newt_error_name(const char *format, ...);

#if NEWT_DEBUG
void
newt_panic(const char *message);
#endif

#ifndef sprintf_const
#define sprintf_const sprintf
#endif

extern bool newt_abort;

/* newt-for.c */

void
newt_range_start(newt_id_t id, newt_offset_t nparam);

bool
newt_range_step(void);

extern const newt_mem_t newt_range_mem;
extern newt_range_t *newt_ranges;

void
newt_in_start(newt_id_t id);

bool
newt_in_step(void);

extern const newt_mem_t newt_in_mem;
extern newt_in_t *newt_ins;

/* newt-frame.c */

extern newt_frame_t	*newt_globals;
extern newt_frame_t	*newt_frame;

extern const newt_mem_t newt_variable_mem;

bool
newt_frame_mark_global(newt_offset_t name);

newt_frame_t *
newt_frame_push(newt_code_t *code, newt_offset_t ip, newt_offset_t nformal);

newt_code_t *
newt_frame_pop(newt_offset_t *ip_p);

newt_poly_t *
newt_id_ref(newt_id_t id, bool insert);

bool
newt_id_del(newt_id_t id);

extern const newt_mem_t newt_frame_mem;

/* newt-func.c */

newt_func_t *
newt_func_alloc(newt_code_t *code, newt_offset_t nparam, newt_id_t *formals);

bool
newt_func_push(newt_func_t *func, newt_offset_t nactual, newt_code_t *code, newt_offset_t ip);

newt_code_t *
newt_func_pop(newt_offset_t *ip);

extern const newt_mem_t newt_func_mem;

/* newt-gram.y */

extern bool newt_print_vals;

/* newt-lex.l */

extern int newt_want_indent, newt_current_indent;
extern char *newt_file;
extern int newt_line;
extern int newt_ignore_nl;
extern char newt_lex_text[];

/* newt-list.c */

newt_list_t *
newt_list_make(newt_offset_t size, bool readonly);

newt_list_t *
newt_list_append(newt_list_t *list, newt_list_t *append);

newt_list_t *
newt_list_plus(newt_list_t *a, newt_list_t *b);

bool
newt_list_equal(newt_list_t *a, newt_list_t *b);

newt_list_t *
newt_list_imm(newt_offset_t size, bool readonly);

newt_list_t *
newt_list_slice(newt_list_t *list, newt_slice_t *slice);

extern const newt_mem_t newt_list_mem;

/* newt-lex.c */

extern int newt_lex_indent;

token_t
newt_lex(void);

/* newt-memory.c */

#define NEWT_COLLECT_FULL		0
#define NEWT_COLLECT_INCREMENTAL	1

bool
newt_poly_mark(newt_poly_t p);

bool
newt_poly_mark_ref(newt_poly_t *p);

newt_offset_t
newt_collect(uint8_t style);

bool
newt_mark_blob(void *addr, newt_offset_t size);

bool
newt_mark_block_addr(const struct newt_mem *type, void *addr);

bool
newt_mark_block_offset(const struct newt_mem *type, newt_offset_t offset);

bool
newt_mark_addr(const struct newt_mem *type, void *addr);

bool
newt_mark_offset(const struct newt_mem *type, newt_offset_t offset);

bool
newt_move_block_offset(newt_offset_t *ref);

bool
newt_move_block_addr(void **ref);

bool
newt_poly_move(newt_poly_t *ref);

bool
newt_move_addr(const struct newt_mem *type, void **ref);

bool
newt_move_offset(const struct newt_mem *type, newt_offset_t *ref);

void *
newt_alloc(newt_offset_t size);

void
newt_poly_stash(newt_poly_t p);

newt_poly_t
newt_poly_fetch(void);

void
newt_code_stash(newt_code_t *code);

newt_code_t *
newt_code_fetch(void);

/* newt-name.c */

newt_id_t
newt_name_id(char *name, bool *keyword);

const char *
newt_name_string(newt_id_t id);

extern const newt_mem_t newt_name_mem;
extern newt_name_t *newt_names;

/* newt-parse.c */

typedef enum {
	newt_parse_success,
	newt_parse_error,
} newt_parse_ret_t;

newt_parse_ret_t
newt_parse(void);

/* newt-poly.c */

void *
newt_ref(newt_poly_t poly);

newt_poly_t
newt_poly_offset(newt_offset_t offset, newt_type_t type);

newt_poly_t
newt_poly(const void *addr, newt_type_t type);

void
newt_poly_print(FILE *file, newt_poly_t poly);

bool
newt_poly_equal(newt_poly_t a, newt_poly_t b);

bool
newt_poly_true(newt_poly_t a);

int
newt_poly_len(newt_poly_t a);

bool
newt_slice_canon(newt_slice_t *slice);

newt_offset_t
newt_null_size(void *addr);

void
newt_null_mark_move(void *addr);

extern const newt_mem_t newt_null_mem;

char *
newt_poly_format(newt_poly_t a, char format);

/* newt-string.c */

char *
newt_string_make(char c);

char
newt_string_fetch(char *string, int i);

char *
newt_string_cat(char *a, char *b);

char *
newt_string_slice(char *a, newt_slice_t *slice);

char *
newt_string_interpolate(char *a, newt_poly_t poly);

extern const newt_mem_t newt_string_mem;

/* inlines */

static inline void
newt_stack_push(newt_poly_t p)
{
	if (newt_stackp == NEWT_STACK) {
		newt_error("stack overflow");
		return;
	}
	newt_stack[newt_stackp++] = p;
}

static inline newt_poly_t
newt_stack_pop(void)
{
#if NEWT_DEBUG
	if (!newt_stackp)
		newt_panic("stack underflow");
#endif
	return newt_stack[--newt_stackp];
}

static inline newt_poly_t
newt_stack_pick(newt_offset_t off)
{
	return newt_stack[newt_stackp - off - 1];
}

static inline void
newt_stack_drop(newt_offset_t off)
{
	newt_stackp -= off;
}

static inline void
newt_slice_start(newt_slice_t *slice)
{
	slice->pos = slice->start;
}

static inline bool
newt_slice_test(newt_slice_t *slice)
{
	return slice->stride > 0 ? slice->pos < slice->end : slice->pos > slice->end;
}

static inline void
newt_slice_step(newt_slice_t *slice)
{
	slice->pos += slice->stride;
}

static inline bool
newt_slice_identity(newt_slice_t *slice)
{
	return slice->start == 0 && slice->end == slice->len && slice->stride == 1;
}

static inline void *
newt_pool_ref(newt_offset_t offset)
{
	if (offset == 0)
		return NULL;

#if NEWT_DEBUG
	if (((offset - 1) & (NEWT_ALLOC_ROUND-1)) != 0)
		newt_panic("bad offset");
#endif

	return newt_pool + offset - 1;
}

static inline newt_offset_t
newt_pool_offset(const void *addr)
{
	if (addr == NULL)
		return 0;

#if NEWT_DEBUG
	if (((uintptr_t) addr & (NEWT_ALLOC_ROUND-1)) != 0)
		newt_panic("bad address");
#endif

	return ((const uint8_t *) addr) - newt_pool + 1;
}

static inline bool
newt_is_float(newt_poly_t v)
{
	if ((v.u & 0xff000000) != 0xff000000 || v.u == NEWT_NAN_U)
		return true;
	return false;
}

static inline newt_poly_t
newt_float_to_poly(float f)
{
	if (isnanf(f))
		return NEWT_NAN;
	return (newt_poly_t) { .f = f };
}

static inline newt_poly_t
newt_offset_to_poly(newt_offset_t offset, newt_type_t type)
{
	return (newt_poly_t) { .u = 0xff000000 | offset | type };
}

static inline newt_offset_t
newt_poly_to_offset(newt_poly_t v)
{
	return v.u & NEWT_OFFSET_MASK;
}

static inline float
newt_poly_to_float(newt_poly_t v)
{
	return v.f;
}

static inline int
newt_is_pool_addr(const void *addr) {
	const uint8_t *a = addr;
	return (newt_pool <= a) && (a < newt_pool + NEWT_POOL);
}

static inline newt_type_t
newt_poly_type(newt_poly_t v)
{
	return newt_is_float(v) ? newt_float : (v.u & 3);
}

static inline int
newt_size_round(int size)
{
	return (size + (NEWT_ALLOC_ROUND - 1)) & ~(NEWT_ALLOC_ROUND - 1);
}

static inline int
newt_size(const newt_mem_t *mem, void *addr)
{
	return newt_size_round(NEWT_MEM_SIZE(mem)(addr));
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

static inline newt_poly_t *
newt_list_data(newt_list_t *list)
{
	return newt_pool_ref(list->data);
}

static inline newt_poly_t
newt_string_to_poly(char *string)
{
	return newt_poly(string, newt_string);
}

static inline char *
newt_poly_to_string(newt_poly_t poly)
{
	return newt_ref(poly);
}

static inline newt_func_t *
newt_poly_to_func(newt_poly_t poly)
{
	return newt_ref(poly);
}

static inline newt_poly_t
newt_func_to_poly(newt_func_t *func)
{
	return newt_poly(func, newt_func);
}

static inline newt_poly_t
newt_builtin_id_to_poly(newt_id_t id)
{
	return newt_offset_to_poly(id << NEWT_ALLOC_SHIFT, newt_builtin);
}

static inline newt_id_t
newt_poly_to_builtin_id(newt_poly_t a)
{
	return newt_poly_to_offset(a) >> NEWT_ALLOC_SHIFT;
}

static inline const newt_builtin_t *
newt_poly_to_builtin(newt_poly_t a)
{
	return newt_builtins + (newt_poly_to_builtin_id(a) - 1);
}

static inline newt_poly_t
newt_bool_to_poly(bool b)
{
	return b ? NEWT_ONE : NEWT_ZERO;
}

static inline bool
newt_list_readonly(newt_list_t *list)
{
	return (list->note_next_and_readonly & 1) != 0;
}

static inline bool
newt_list_noted(newt_list_t *list)
{
	return (list->note_next_and_readonly & 2) != 0;
}

static inline newt_offset_t
newt_list_note_next(newt_list_t *list)
{
	newt_offset_t offset = list->note_next_and_readonly & ~3;
	if (offset)
		offset++;
	return offset;
}

static inline void
newt_list_set_readonly(newt_list_t *list, bool readonly)
{
	list->note_next_and_readonly = (list->note_next_and_readonly & ~1) | (readonly ? 1 : 0);
}

static inline void
newt_list_set_noted(newt_list_t *list, bool noted)
{
	list->note_next_and_readonly = (list->note_next_and_readonly & ~3) | (noted ? 2 : 0);
}

static inline void
newt_list_set_note_next(newt_list_t *list, newt_offset_t note_next)
{
	note_next -= note_next & 1;
#if NEWT_DEBUG
	if (note_next & 3)
		newt_panic("note_next bad alignment");
#endif
	list->note_next_and_readonly = (list->note_next_and_readonly & 3) | (note_next);
}

static inline newt_offset_t
newt_code_current(void)
{
	return newt_compile_size;
}

static inline newt_offset_t
newt_code_prev_insn(void)
{
	return newt_compile_prev;
}

static inline newt_offset_t
newt_code_prev_prev_insn(void)
{
	return newt_compile_prev_prev;
}

static inline uint8_t *
newt_code_at(newt_offset_t offset)
{
	return newt_compile + offset;
}

static inline void
newt_code_set_push(newt_offset_t offset)
{
	newt_compile[offset] |= newt_op_push;
}

static inline void
newt_code_delete_prev(void)
{
	newt_compile_size = newt_compile_prev;
	newt_compile_prev = newt_compile_prev_prev;
}
