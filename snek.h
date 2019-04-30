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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "snek-builtin.h"

// #define DEBUG_MEMORY

#ifdef DEBUG_MEMORY
#define debug_memory(fmt, args...) printf(fmt, ## args)
#else
#define debug_memory(fmt, args...)
#endif

#ifndef SNEK_POOL
#define SNEK_POOL		(32 * 1024)
#endif
#define SNEK_ALLOC_SHIFT	2
#define SNEK_ALLOC_ROUND	(1 << SNEK_ALLOC_SHIFT)

/*
 * Offsets are encoded in floats as NaN values with the sign bit
 * set. That means the top 9 bits of the value are all one. There is
 * one other value with that value, -inf, which is encoded as negative
 * sign, all exponent bits one and a zero mantissa.
 *
 * This leaves us 23 bits for offsets. The bottom two bits of offsets
 * are used as tags; allocations within the heap are rounded to a
 * multiple of four bytes so that offsets will always have the bottom
 * two bits clear.
 */

#define SNEK_OFFSET_MASK	0x007ffffcu
#define SNEK_EXPONENT_MASK	0xff800000u
#define SNEK_NINF		0xff800000u

#if SNEK_POOL <= 65536
typedef uint16_t	snek_offset_t;
typedef int16_t		snek_soffset_t;
#define SNEK_OFFSET_NONE	0xfffcu
#define SNEK_SOFFSET_NONE	0x7ffc
#else
typedef uint32_t	snek_offset_t;
typedef int32_t		snek_soffset_t;
#define SNEK_OFFSET_NONE	0xfffffffcu
#define SNEK_SOFFSET_NONE	0x7ffffffc
#endif

typedef snek_offset_t snek_id_t;

#define SNEK_ID_NONE		0

typedef union {
	uint32_t    u;
	float       f;
} snek_poly_t;

typedef enum {
	snek_op_eq = 0,
	snek_op_ne = 1,
	snek_op_gt = 2,
	snek_op_lt = 3,
	snek_op_ge = 4,
	snek_op_le = 5,

	snek_op_is = 6,
	snek_op_is_not = 7,
	snek_op_in = 8,
	snek_op_not_in = 9,

	snek_op_array = 10,

	snek_op_plus = 11,
	snek_op_minus = 12,
	snek_op_times = 13,
	snek_op_divide = 14,
	snek_op_div = 15,
	snek_op_mod = 16,
	snek_op_pow = 17,
	snek_op_land = 18,
	snek_op_lor = 19,
	snek_op_lxor = 20,
	snek_op_lshift = 21,
	snek_op_rshift = 22,

	snek_op_assign_plus = 23,
	snek_op_assign_minus = 24,
	snek_op_assign_times = 25,
	snek_op_assign_divide = 26,
	snek_op_assign_div = 27,
	snek_op_assign_mod = 28,
	snek_op_assign_pow = 29,
	snek_op_assign_land = 30,
	snek_op_assign_lor = 31,
	snek_op_assign_lxor = 32,
	snek_op_assign_lshift = 33,
	snek_op_assign_rshift = 34,

	snek_op_assign = 35,
	snek_op_assign_named = 36,

	snek_op_num,
	snek_op_int,
	snek_op_string,
	snek_op_list,
	snek_op_tuple,
	snek_op_dict,
	snek_op_id,

	snek_op_not,
	snek_op_uminus,
	snek_op_lnot,

	snek_op_call,

	snek_op_slice,

	snek_op_global,
	snek_op_del,

	snek_op_branch,
	snek_op_branch_true,
	snek_op_branch_false,
	snek_op_forward,
	snek_op_range_start,
	snek_op_range_step,
	snek_op_in_step,

	snek_op_line,

	snek_op_null,

	snek_op_nop,

	snek_op_push = 0x80,
} __attribute__((packed)) snek_op_t;

typedef enum {
	snek_forward_return,
	snek_forward_break,
	snek_forward_continue,
	snek_forward_if,
} __attribute__((packed)) snek_forward_t;

typedef enum {
	snek_builtin = 0,
	snek_list = 1,
	snek_string = 2,
	snek_func = 3,
	snek_float = 4,
} __attribute__((packed)) snek_type_t;

typedef struct snek_mem {
	snek_offset_t	(*size)(void *addr);
	void		(*mark)(void *addr);
	void		(*move)(void *addr);
#ifdef SNEK_MEM_INCLUDE_NAME
	char		name[16];
#endif
} snek_mem_t;

#ifndef SNEK_MEM_DECLARE
#define SNEK_MEM_DECLARE(n) n
#define SNEK_MEM_SIZE(m)	((m)->size)
#define SNEK_MEM_MARK(m)	((m)->mark)
#define SNEK_MEM_MOVE(m)	((m)->move)
#endif

#ifndef SNEK_MEMS_DECLARE
#define SNEK_MEMS_DECLARE(n) n
#endif

#ifdef SNEK_MEM_INCLUDE_NAME
#define SNEK_MEM_DECLARE_NAME(_name)	.name = _name,
#else
#define SNEK_MEM_DECLARE_NAME(_name)
#endif

typedef enum {
	snek_list_list,
	snek_list_tuple,
	snek_list_dict
} __attribute__((packed)) snek_list_type_t;

typedef struct snek_list {
	snek_offset_t	size;
	snek_offset_t	alloc;
	snek_offset_t	note_next_and_type;
	snek_offset_t	data;
} snek_list_t;

typedef struct snek_code {
	snek_offset_t	size;
	uint8_t		code[0];
} snek_code_t;

typedef struct snek_range {
	snek_offset_t	prev;
	snek_id_t	id;
	float		current;
	float		limit;
	float		step;
} snek_range_t;

typedef struct snek_in {
	snek_offset_t	prev;
	snek_id_t	id;
	snek_poly_t	array;
	snek_offset_t	i;
} snek_in_t;

typedef struct snek_func {
	snek_soffset_t	nformal;
	snek_offset_t	code;
	snek_id_t	formals[0];
} snek_func_t;

#define SNEK_FUNC_VARARGS	SNEK_SOFFSET_NONE

typedef struct snek_name {
	snek_offset_t	next;
	char		name[0];
} snek_name_t;

typedef struct snek_variable {
	snek_id_t	id;
	snek_poly_t	value;
} snek_variable_t;

typedef struct snek_frame {
	snek_offset_t	prev;
	snek_offset_t	code;
	snek_offset_t	ip;
	snek_offset_t	nvariables;
	snek_variable_t	variables[0];
} snek_frame_t;


typedef struct snek_slice {
	/* number of outputs */
	snek_offset_t	count;

	/* position of current object */
	snek_offset_t	pos;

	/* stride between inputs */
	snek_soffset_t	stride;

	/* slice is identity */
	bool		identity;
} snek_slice_t;

#define SNEK_SLICE_DEFAULT	SNEK_SOFFSET_NONE		/* empty value provided [1:] */

typedef struct snek_builtin {
	union {
		struct {
			int8_t nformal;
			union {
				snek_poly_t	(*funcv)(uint8_t nposition, uint8_t nnamed,
							 snek_poly_t *args);
				snek_poly_t	(*func0)(void);
				snek_poly_t	(*func1)(snek_poly_t a0);
				snek_poly_t	(*func2)(snek_poly_t a0, snek_poly_t a1);
				snek_poly_t	(*func3)(snek_poly_t a0, snek_poly_t a1, snek_poly_t a2);
				snek_poly_t	(*func4)(snek_poly_t a0, snek_poly_t a1, snek_poly_t a2, snek_poly_t a3);
			};
		};
		float		value;
	};
} snek_builtin_t;

typedef struct snek_buf {
	int	(*put_c)(int c, void *closure);
	int	(*put_s)(const char *s, void *closure);
	void	*closure;
} snek_buf_t;

extern const snek_builtin_t snek_builtins[];

#define SNEK_BUILTIN_FLOAT	-2
#define SNEK_BUILTIN_VARARGS	-1

#define SNEK_NAN_U	0x7fffffffu
#define SNEK_NAN	((snek_poly_t) { .u = SNEK_NAN_U })
#define SNEK_NULL_U	0xfffffffcu
#define SNEK_NULL	((snek_poly_t) { .u = SNEK_NULL_U })
#define SNEK_GLOBAL_U	0xfffffff8u
#define SNEK_GLOBAL	((snek_poly_t) { .u = SNEK_GLOBAL_U })
#define SNEK_ZERO	((snek_poly_t) { .f = 0.0f })
#define SNEK_ONE	((snek_poly_t) { .f = 1.0f })

#ifndef SNEK_STACK
#define SNEK_STACK	256
#endif
extern snek_poly_t	snek_stack[SNEK_STACK];
extern snek_offset_t	snek_stackp;
extern snek_poly_t	snek_a;
extern snek_code_t	*snek_code;

static inline bool
snek_is_nan(snek_poly_t p)
{
	return p.u == SNEK_NAN_U;
}

static inline bool
snek_is_null(snek_poly_t p)
{
	return p.u == SNEK_NULL_U;
}

static inline bool
snek_is_global(snek_poly_t p)
{
	return p.u == SNEK_GLOBAL_U;
}


#ifdef SNEK_DYNAMIC
extern uint8_t *snek_pool  __attribute__((aligned(SNEK_ALLOC_ROUND)));
extern uint32_t	snek_pool_size;
#else
extern uint8_t	snek_pool[SNEK_POOL] __attribute__((aligned(SNEK_ALLOC_ROUND)));
#endif

#ifndef SNEK_CODE_HOOK_START
#define SNEK_CODE_HOOK_START
#endif

#ifndef SNEK_CODE_HOOK_STOP
#define SNEK_CODE_HOOK_STOP
#endif

#include "snek-gram.h"

typedef union {
	bool		bools;
	int16_t		_ints;
	uint8_t		indent;
	snek_offset_t	line;
	snek_op_t	op;
	snek_offset_t	offset;
	snek_soffset_t	soffset;
	snek_id_t	id;
	float		number;
	char		*string;
} snek_token_val_t;

extern snek_token_val_t	snek_token_val;

/* snek-builtin.c */

#define SNEK_BUILTIN_DECLS
#include "snek-builtin.h"

/* snek-code.c */

extern const char * const snek_op_names[];

extern uint8_t		*snek_compile;
extern snek_offset_t	snek_compile_size;
extern snek_offset_t	snek_compile_prev, snek_compile_prev_prev;

#define SNEK_OP_SLICE_START	1
#define SNEK_OP_SLICE_END	2
#define SNEK_OP_SLICE_STRIDE	4

void
snek_code_delete_prev(void);

void
snek_code_add_op(snek_op_t op);

void
snek_code_add_op_array(snek_op_t op);

void
snek_code_add_number(float number);

void
snek_code_add_string(char *string);

void
snek_code_add_op_offset(snek_op_t op, snek_offset_t offset);

static inline void
snek_code_add_op_id(snek_op_t op, snek_id_t id)
{
	snek_code_add_op_offset(op, id);
}

void
snek_code_add_op_branch(snek_op_t op);

void
snek_code_add_forward(snek_forward_t forward);

void
snek_code_patch_forward(snek_offset_t start, snek_offset_t stop, snek_forward_t forward, snek_offset_t target);

void
snek_code_add_slice(bool has_start, bool has_end, bool has_stride);

void
snek_code_add_in_range(snek_id_t id, snek_offset_t nactual, uint8_t for_depth);

void
snek_code_add_in_enum(snek_id_t id, uint8_t for_depth);

void
snek_code_patch_branch(snek_offset_t branch, snek_offset_t target);

snek_code_t *
snek_code_finish(void);

snek_offset_t
snek_code_line(snek_code_t *code);

float
snek_poly_get_float(snek_poly_t a);

snek_soffset_t
snek_poly_get_soffset(snek_poly_t a);

snek_soffset_t
snek_stack_pop_soffset(void);

float
snek_stack_pop_float(void);

void
snek_stack_push(snek_poly_t p);

snek_poly_t
snek_stack_pop(void);

snek_poly_t
snek_stack_pick(snek_offset_t off);

void
snek_stack_drop(snek_offset_t off);

void
snek_run_mark(void);

void
snek_run_move(void);

snek_poly_t
snek_code_run(snek_code_t *code);

void
snek_undefined(snek_id_t id);

extern const snek_mem_t snek_code_mem;
extern const snek_mem_t snek_compile_mem;

/* snek-error.c */

#ifndef snek_error_name
#define snek_error_name snek_error
#define snek_error_0_name snek_error_0
#endif

snek_poly_t
snek_error_name(const char *format, ...);

snek_poly_t
snek_error_0_name(const char *string);

snek_poly_t
snek_error_step(void);

snek_poly_t
snek_error_value(snek_poly_t p);

snek_poly_t
snek_error_type_2(snek_poly_t a, snek_poly_t b);

snek_poly_t
snek_error_type_1(snek_poly_t a);

#if SNEK_DEBUG
void
snek_panic(const char *message);
#endif

#ifndef sprintf_const
#define sprintf_const sprintf
#endif

extern bool snek_abort;

/* snek-frame.c */

extern snek_frame_t	*snek_globals;
extern snek_frame_t	*snek_frame;

bool
snek_frame_mark_global(snek_offset_t name);

bool
snek_frame_push(snek_offset_t ip, snek_offset_t nformal);

snek_offset_t
snek_frame_pop(void);

snek_poly_t *
snek_id_ref(snek_id_t id, bool insert);

bool
snek_id_is_local(snek_id_t id);

bool
snek_id_del(snek_id_t id);

extern const snek_mem_t snek_frame_mem;

/* snek-func.c */

extern snek_code_t	*snek_stash_code;

snek_func_t *
snek_func_alloc(snek_code_t *code);

bool
snek_func_push(uint8_t nposition, uint8_t nnamed, snek_offset_t ip);

snek_code_t *
snek_func_pop(snek_offset_t *ip);

snek_offset_t
snek_func_size(void *addr);

void
snek_func_mark(void *addr);

void
snek_func_move(void *addr);

/* snek-lex.l */

extern uint8_t snek_current_indent;
extern char *snek_file;
extern snek_offset_t snek_line;
extern snek_offset_t snek_lex_line;
extern uint8_t snek_ignore_nl;
extern bool snek_parse_middle;
extern bool snek_interactive;
extern char snek_lex_text[];

/* snek-list.c */

extern snek_list_t *snek_empty_tuple;

snek_list_t *
snek_list_resize(snek_list_t *list, snek_offset_t size);

snek_list_t *
snek_list_make(snek_offset_t size, snek_list_type_t type);

snek_list_t *
snek_list_append(snek_list_t *list, snek_list_t *append);

snek_list_t *
snek_list_plus(snek_list_t *a, snek_list_t *b);

snek_list_t *
snek_list_times(snek_list_t *a, snek_soffset_t count);

snek_poly_t *
snek_list_ref(snek_list_t *list, snek_poly_t p, bool report_error);

snek_poly_t
snek_list_get(snek_list_t *list, snek_poly_t p, bool report_error);

snek_poly_t *
snek_list_data(snek_list_t *list);

void
snek_list_del(snek_poly_t lp, snek_poly_t p);

int8_t
snek_list_cmp(snek_list_t *a, snek_list_t *b);

snek_poly_t
snek_list_imm(snek_offset_t size, snek_list_type_t type);

snek_list_t *
snek_list_slice(snek_list_t *list, snek_slice_t *slice);

snek_offset_t
snek_list_size(void *addr);

void
snek_list_mark(void *addr);

void
snek_list_move(void *addr);

#ifdef SNEK_LIST_BUILD
snek_poly_t
snek_list_build(snek_list_type_t type, snek_offset_t size, ...);
#endif

/* snek-lex.c */

extern uint8_t snek_lex_indent;

token_t
snek_lex(void);

/* snek-memory.c */

#define SNEK_COLLECT_FULL		0
#define SNEK_COLLECT_INCREMENTAL	1

bool
snek_is_pool_addr(const void *addr);

bool
snek_poly_mark(snek_poly_t p);

bool
snek_poly_mark_ref(snek_poly_t *p);

snek_offset_t
snek_collect(uint8_t style);

bool
snek_mark_blob(void *addr, snek_offset_t size);

bool
snek_mark_block_addr(const struct snek_mem *type, void *addr);

bool
snek_mark_addr(const struct snek_mem *type, void *addr);

bool
snek_mark_offset(const struct snek_mem *type, snek_offset_t offset);

bool
snek_move_block_offset(void *ref);

bool
snek_move_block_addr(void **ref);

bool
snek_poly_move(snek_poly_t *ref);

bool
snek_move_addr(const struct snek_mem *type, void **ref);

bool
snek_move_offset(const struct snek_mem *type, snek_offset_t *ref);

void *
snek_alloc(snek_offset_t size);

void
snek_stack_push_string(const char *s);

char *
snek_stack_pop_string(const char *s);

void
snek_stack_push_list(snek_list_t *list);

snek_list_t *
snek_stack_pop_list(void);

void *
snek_pool_addr(snek_offset_t offset);

snek_offset_t
snek_pool_offset(const void *addr);

extern const struct snek_mem SNEK_MEMS_DECLARE(_snek_mems)[];

static inline const struct snek_mem *snek_mems(snek_type_t type)
{
	/* Oddly, the compiler complains about this particular array
	 * operation. However, this lets us declare _snek_mems with
	 * one fewer entry, without causing any increase in code size
	 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
	return &((&_snek_mems[-1])[type]);
#pragma GCC diagnostic pop
}

/* snek-name.c */

snek_id_t
snek_name_id(char *name, bool *keyword);

const char *
snek_name_string(snek_id_t id);

extern const snek_mem_t snek_name_mem;
extern snek_name_t *snek_names;

/* snek-parse.c */

#define SNEK_MAX_FORMALS	10

extern uint8_t snek_parse_nformal;
extern snek_id_t snek_parse_formals[SNEK_MAX_FORMALS];

typedef enum {
	snek_parse_success,
	snek_parse_error,
} snek_parse_ret_t;

snek_parse_ret_t
snek_parse(void);

/* snek-poly.c */

void *
snek_ref(snek_poly_t poly);

snek_poly_t
snek_poly_offset(snek_offset_t offset, snek_type_t type);

snek_poly_t
snek_poly(const void *addr, snek_type_t type);

snek_poly_t
snek_float_to_poly(float f);

snek_type_t
snek_poly_type(snek_poly_t v);

void
snek_poly_print(FILE *file, snek_poly_t poly, char format);

int8_t
snek_poly_cmp(snek_poly_t a, snek_poly_t b, bool is);

bool
snek_poly_true(snek_poly_t a);

snek_offset_t
snek_poly_len(snek_poly_t a);

void
snek_poly_format(snek_buf_t *buf, snek_poly_t a, char format);

/* snek-print.c */
void
snek_print(snek_buf_t *buf, snek_poly_t poly);

/* snek-string.c */

char *
snek_string_make(char c);

snek_poly_t
snek_string_get(char *string, snek_poly_t p, bool report_error);

snek_poly_t
snek_string_cat(char *a, char *b);

char *
snek_string_slice(char *a, snek_slice_t *slice);

char *
snek_string_interpolate(char *a, snek_poly_t poly);

snek_offset_t
snek_string_size(void *addr);

void
snek_string_mark_move(void *addr);

/* inlines */

static inline bool
snek_slice_test(snek_slice_t *slice)
{
	return slice->count != 0;
}

static inline void
snek_slice_step(snek_slice_t *slice)
{
	slice->count--;
	slice->pos += slice->stride;
}

static inline snek_poly_t
snek_offset_to_poly(snek_offset_t offset, snek_type_t type)
{
	return (snek_poly_t) { .u = SNEK_EXPONENT_MASK | offset | type };
}

static inline snek_offset_t
snek_poly_to_offset(snek_poly_t v)
{
	return v.u & SNEK_OFFSET_MASK;
}

static inline float
snek_poly_to_float(snek_poly_t v)
{
	return v.f;
}

static inline snek_poly_t
snek_list_to_poly(snek_list_t *list)
{
	return snek_poly(list, snek_list);
}

static inline snek_list_t *
snek_poly_to_list(snek_poly_t poly)
{
	return snek_ref(poly);
}

static inline snek_poly_t
snek_string_to_poly(char *string)
{
	return snek_poly(string, snek_string);
}

static inline char *
snek_poly_to_string(snek_poly_t poly)
{
	return snek_ref(poly);
}

static inline snek_func_t *
snek_poly_to_func(snek_poly_t poly)
{
	return snek_ref(poly);
}

static inline snek_poly_t
snek_func_to_poly(snek_func_t *func)
{
	return snek_poly(func, snek_func);
}

static inline snek_poly_t
snek_builtin_id_to_poly(snek_id_t id)
{
	if (id < SNEK_BUILTIN_MAX_FUNC)
		return snek_offset_to_poly(id << SNEK_ALLOC_SHIFT, snek_builtin);
	return snek_float_to_poly(SNEK_BUILTIN_VALUE(&snek_builtins[id-1]));
}

static inline snek_id_t
snek_poly_to_builtin_id(snek_poly_t a)
{
	return snek_poly_to_offset(a) >> SNEK_ALLOC_SHIFT;
}

static inline const snek_builtin_t *
snek_poly_to_builtin(snek_poly_t a)
{
	return snek_builtins + (snek_poly_to_builtin_id(a) - 1);
}

static inline bool
snek_offset_is_none(snek_offset_t offset)
{
	return offset == SNEK_OFFSET_NONE;
}

snek_poly_t
snek_bool_to_poly(bool b);

static inline snek_offset_t
snek_offset_value(snek_offset_t offset)
{
	offset = offset & ~3;
	return offset;
}

static inline bool
snek_offset_flag_0(snek_offset_t offset)
{
	return offset & 1;
}

static inline bool
snek_offset_flag_1(snek_offset_t offset)
{
	return !!(offset & 2);
}

static inline uint8_t
snek_offset_flags(snek_offset_t offset)
{
	return offset & 3;
}

static inline snek_offset_t
snek_offset_set_flag_0(snek_offset_t offset, bool flag)
{
	return (offset & ~1) | (flag ? 1 : 0);
}

static inline snek_offset_t
snek_offset_set_flag_1(snek_offset_t offset, bool flag)
{
	return (offset & ~2) | (flag ? 2 : 0);
}

static inline snek_offset_t
snek_offset_set_flags(snek_offset_t offset, uint8_t flags)
{
	return offset | flags;
}

static inline snek_offset_t
snek_offset_set_value(snek_offset_t offset, snek_offset_t value)
{
#if SNEK_DEBUG
	if (value & 3)
		snek_panic("note_next bad alignment");
#endif
	return value | (offset & 3);
}

static inline bool
snek_list_readonly(snek_list_t *list)
{
	return snek_offset_flags(list->note_next_and_type) == snek_list_tuple;
}

static inline snek_list_type_t
snek_list_type(snek_list_t *list)
{
	return snek_offset_flags(list->note_next_and_type);
}

static inline snek_offset_t
snek_list_note_next(snek_list_t *list)
{
	return snek_offset_value(list->note_next_and_type);
}

static inline void
snek_list_set_type(snek_list_t *list, uint8_t type)
{
	list->note_next_and_type = snek_offset_set_flags(list->note_next_and_type, type);
}

static inline void
snek_list_set_note_next(snek_list_t *list, snek_offset_t note_next)
{
	list->note_next_and_type = snek_offset_set_value(list->note_next_and_type, note_next);
}

static inline snek_offset_t
snek_code_current(void)
{
	return snek_compile_size;
}

static inline snek_offset_t
snek_code_prev_insn(void)
{
	return snek_compile_prev;
}

static inline snek_offset_t
snek_code_prev_prev_insn(void)
{
	return snek_compile_prev_prev;
}

static inline uint8_t *
snek_code_at(snek_offset_t offset)
{
	return snek_compile + offset;
}

static inline void
snek_code_set_push(snek_offset_t offset)
{
	snek_compile[offset] |= snek_op_push;
}

