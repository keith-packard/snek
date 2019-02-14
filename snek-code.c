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

#include "snek.h"

static uint8_t
snek_op_extra_size(snek_op_t op)
{
	switch (op) {
	case snek_op_num:
		return sizeof (float);
	case snek_op_int:
		return sizeof (int8_t);
	case snek_op_string:
	case snek_op_list:
	case snek_op_tuple:
		return sizeof (snek_offset_t);
	case snek_op_id:
	case snek_op_global:
	case snek_op_assign:
	case snek_op_assign_named:
	case snek_op_assign_plus:
	case snek_op_assign_minus:
	case snek_op_assign_times:
	case snek_op_assign_divide:
	case snek_op_assign_div:
	case snek_op_assign_mod:
	case snek_op_assign_pow:
	case snek_op_assign_land:
	case snek_op_assign_lor:
	case snek_op_assign_lxor:
	case snek_op_assign_lshift:
	case snek_op_assign_rshift:
		return sizeof (snek_id_t);
	case snek_op_call:
		return sizeof (snek_offset_t);
	case snek_op_slice:
		return 1;
	case snek_op_branch:
	case snek_op_branch_true:
	case snek_op_branch_false:
	case snek_op_forward:
	case snek_op_line:
		return sizeof (snek_offset_t);
	case snek_op_range_start:
	case snek_op_range_step:
	case snek_op_in_step:
		return sizeof (snek_offset_t) + sizeof (uint8_t) + sizeof (snek_id_t);
	default:
		return 0;
	}
}

//#define DEBUG_COMPILE
//#define DEBUG_EXEC

#define dbg(a, args...) fprintf(stderr, a, ##args)
#define stddbg stderr

#if defined(DEBUG_COMPILE) || defined(DEBUG_EXEC)

const char * const snek_op_names[] = {
	[snek_op_plus] = "plus",
	[snek_op_minus] = "minus",
	[snek_op_times] = "times",
	[snek_op_divide] = "divide",
	[snek_op_div] = "div",
	[snek_op_mod] = "mod",
	[snek_op_pow] = "pow",
	[snek_op_land] = "land",
	[snek_op_lor] = "lor",
	[snek_op_lxor] = "lxor",
	[snek_op_lshift] = "lshift",
	[snek_op_rshift] = "rshift",

	[snek_op_assign_plus] = "assign_plus",
	[snek_op_assign_minus] = "assign_minus",
	[snek_op_assign_times] = "assign_times",
	[snek_op_assign_divide] = "assign_divide",
	[snek_op_assign_div] = "assign_div",
	[snek_op_assign_mod] = "assign_mod",
	[snek_op_assign_pow] = "assign_pow",
	[snek_op_assign_land] = "assign_land",
	[snek_op_assign_lor] = "assign_lor",
	[snek_op_assign_lxor] = "assign_lxor",
	[snek_op_assign_lshift] = "assign_lshift",
	[snek_op_assign_rshift] = "assign_rshift",

	[snek_op_num] = "num",
	[snek_op_int] = "int",
	[snek_op_string] = "string",
	[snek_op_list] = "list",
	[snek_op_tuple] = "tuple",
	[snek_op_id] = "id",


	[snek_op_not] = "not",

	[snek_op_eq] = "eq",
	[snek_op_ne] = "ne",
	[snek_op_gt] = "gt",
	[snek_op_lt] = "lt",
	[snek_op_ge] = "ge",
	[snek_op_le] = "le",

	[snek_op_is] = "is",
	[snek_op_is_not] = "is_not",
	[snek_op_in] = "in",
	[snek_op_not_in] = "not_in",

	[snek_op_uminus] = "uminus",
	[snek_op_lnot] = "lnot",

	[snek_op_call] = "call",

	[snek_op_array] = "array",
	[snek_op_slice] = "slice",

	[snek_op_assign] = "assign",
	[snek_op_assign_named] = "assign_named",

	[snek_op_global] = "global",

	[snek_op_branch] = "branch",
	[snek_op_branch_true] = "branch_true",
	[snek_op_branch_false] = "branch_false",
	[snek_op_forward] = "forward",
	[snek_op_range_start] = "range_start",
	[snek_op_range_step] = "range_step",
	[snek_op_in_step] = "in_step",
	[snek_op_nop] = "nop",
	[snek_op_line] = "line",
};

static snek_offset_t
snek_code_dump_instruction(snek_code_t *code, snek_offset_t ip)
{
	float		f;
	snek_id_t	id;
	snek_offset_t	o;
	int8_t		i8;
	uint8_t		u8;

	dbg("%6d:  ", ip);
	snek_op_t op = code->code[ip++];
	bool push = (op & snek_op_push) != 0;
	op &= ~snek_op_push;
	dbg("%-12s %c ", snek_op_names[op], push ? '^' : ' ');
	switch(op) {
	case snek_op_num:
		memcpy(&f, &code->code[ip], sizeof(float));
		dbg("%.7g\n", f);
		break;
	case snek_op_int:
		memcpy(&i8, &code->code[ip], sizeof(int8_t));
		dbg("%d\n", i8);
		break;
	case snek_op_string:
		memcpy(&o, &code->code[ip], sizeof(snek_offset_t));
		dbg("%s\n", (char *) snek_pool_ref(o));
		break;
	case snek_op_list:
	case snek_op_tuple:
		memcpy(&o, &code->code[ip], sizeof(snek_offset_t));
		dbg("%u\n", o);
		break;
	case snek_op_id:
	case snek_op_global:
	case snek_op_assign:
	case snek_op_assign_named:
	case snek_op_assign_plus:
	case snek_op_assign_minus:
	case snek_op_assign_times:
	case snek_op_assign_divide:
	case snek_op_assign_div:
	case snek_op_assign_mod:
	case snek_op_assign_pow:
	case snek_op_assign_land:
	case snek_op_assign_lor:
	case snek_op_assign_lxor:
	case snek_op_assign_lshift:
	case snek_op_assign_rshift:
		memcpy(&id, &code->code[ip], sizeof(snek_id_t));
		dbg("(%5d) ", id);
		if (id) {
			const char *name = snek_name_string(id);
			if (!name)
				dbg("<temp %d>\n", id);
			else
				dbg("%s\n", name);
		} else
			dbg("<array>\n");
		break;
	case snek_op_call:
		memcpy(&o, &code->code[ip], sizeof(snek_offset_t));
		dbg("%d actuals\n", o);
		break;
	case snek_op_slice:
		if (code->code[ip] & SNEK_OP_SLICE_START) dbg(" start");
		if (code->code[ip] & SNEK_OP_SLICE_END) dbg(" end");
		if (code->code[ip] & SNEK_OP_SLICE_STRIDE) dbg(" stride");
		break;
	case snek_op_branch:
	case snek_op_branch_true:
	case snek_op_branch_false:
	case snek_op_forward:
	case snek_op_line:
		memcpy(&o, &code->code[ip], sizeof (snek_offset_t));
		dbg("%d\n", o);
		break;
	case snek_op_range_start:
	case snek_op_in_step:
	case snek_op_range_step:
		memcpy(&o, &code->code[ip], sizeof (snek_offset_t));
		memcpy(&u8, &code->code[ip + sizeof (snek_offset_t)], sizeof(uint8_t));
		memcpy(&id, &code->code[ip + sizeof (snek_offset_t) + sizeof(uint8_t)], sizeof(snek_id_t));
		dbg("%d %d %s\n", o, u8, snek_name_string(id));
		break;
	default:
		dbg("\n");
		break;
	}
	return ip + snek_op_extra_size(op);
}

static void
snek_code_dump(snek_code_t *code)
{
	snek_offset_t	ip = 0;

	while (ip < code->size) {
		ip = snek_code_dump_instruction(code, ip);
	}
}

#endif

uint8_t			*snek_compile;
snek_offset_t		snek_compile_size;
snek_offset_t		snek_compile_prev, snek_compile_prev_prev;

static snek_offset_t	compile_alloc;

#define COMPILE_INC	32

static void
compile_extend(snek_offset_t n, void *data)
{
	if (snek_compile_size + n > compile_alloc) {
		uint8_t *new_compile = snek_alloc(compile_alloc + COMPILE_INC);
		if (!new_compile)
			return;
		memcpy(new_compile, snek_compile, snek_compile_size);
		compile_alloc += COMPILE_INC;
		snek_compile = new_compile;
	}
	if (data)
		memcpy(snek_compile + snek_compile_size, data, n);
	snek_compile_size += n;
}

void
snek_code_add_op(snek_op_t op)
{
	snek_compile_prev_prev = snek_compile_prev;
	snek_compile_prev = snek_compile_size;
	compile_extend(1, &op);
}

void
snek_code_add_number(float number)
{
	int8_t i8 = (int8_t) number;
	if ((float) i8 == number) {
		snek_code_add_op(snek_op_int);
		compile_extend(sizeof(int8_t), &i8);
	} else {
		snek_code_add_op(snek_op_num);
		compile_extend(sizeof(float), &number);
	}
}

void
snek_code_add_string(char *string)
{
	snek_offset_t s;
	snek_offset_t strpos;

	snek_stack_push_string(string);
	snek_code_add_op(snek_op_string);
	strpos = snek_compile_size;
	compile_extend(sizeof (snek_offset_t), NULL);
	s = snek_pool_offset(snek_stack_pop_string());
	memcpy(snek_compile + strpos, &s, sizeof (snek_offset_t));
}

void
snek_code_add_op_offset(snek_op_t op, snek_offset_t o)
{
	snek_code_add_op(op);
	compile_extend(sizeof (snek_offset_t), &o);
}

void
snek_code_add_forward(snek_forward_t forward)
{
	snek_code_add_op_offset(snek_op_forward, (snek_offset_t) forward);
}

static inline uint8_t
bit(bool val, uint8_t pos)
{
	return val ? pos : 0;
}

void
snek_code_add_slice(bool has_start, bool has_end, bool has_stride)
{
	snek_code_add_op(snek_op_slice);
	uint8_t param;
	param = (bit(has_start, SNEK_OP_SLICE_START) |
		   bit(has_end,   SNEK_OP_SLICE_END) |
		   bit(has_stride, SNEK_OP_SLICE_STRIDE));
	compile_extend(1, &param);
}

static snek_id_t
snek_for_tmp(uint8_t for_depth, uint8_t i)
{
	return SNEK_OFFSET_NONE - 1 - (for_depth * 2 + i);
}

void
snek_code_add_in_range(snek_id_t id, snek_offset_t nactual, uint8_t for_depth)
{
	snek_code_add_op(snek_op_range_start);
	compile_extend(sizeof (snek_offset_t), &nactual);
	compile_extend(sizeof(uint8_t), &for_depth);
	compile_extend(sizeof (snek_id_t), &id);

	snek_code_add_op(snek_op_range_step);
	compile_extend(sizeof(snek_offset_t), NULL);
	compile_extend(sizeof(uint8_t), &for_depth);
	compile_extend(sizeof (snek_id_t), &id);
}

void
snek_code_add_in_enum(snek_id_t id, uint8_t for_depth)
{
	snek_id_t	list;
	snek_id_t	i;

	list = snek_for_tmp(for_depth, 0);
	i = snek_for_tmp(for_depth, 1);
	snek_code_add_op_id(snek_op_assign, list);
	snek_code_add_number(0.0f);
	snek_code_add_op_id(snek_op_assign, i);
	snek_code_add_op(snek_op_in_step);
	compile_extend(sizeof(snek_offset_t), NULL);
	compile_extend(sizeof(uint8_t), &for_depth);
	compile_extend(sizeof(snek_id_t), &id);
}

void
snek_code_patch_branch(snek_offset_t branch, snek_offset_t target)
{
	memcpy(snek_compile + branch + 1, &target, sizeof (snek_offset_t));
}

void
snek_code_patch_forward(snek_offset_t start, snek_offset_t stop, snek_forward_t forward, snek_offset_t target)
{
	snek_offset_t ip = start;

	while (ip < stop) {
		snek_op_t op = snek_compile[ip++];
		snek_op_t push = op & snek_op_push;
		op &= ~snek_op_push;
		snek_offset_t f;
		switch (op) {
		case snek_op_forward:
			memcpy(&f, &snek_compile[ip], sizeof (snek_offset_t));
			if ((snek_forward_t) f == forward) {
				snek_compile[ip-1] = snek_op_branch | push;
				memcpy(&snek_compile[ip], &target, sizeof(snek_offset_t));
			}
			break;
		default:
			break;
		}
		ip += snek_op_extra_size(op);
	}
}

snek_code_t *
snek_code_finish(void)
{
	if (snek_compile_size == 0)
		return NULL;
	snek_code_patch_forward(0, snek_compile_size, snek_forward_return, snek_code_current());
	snek_code_t *code = snek_alloc(sizeof (snek_code_t) + snek_compile_size);

	if (code) {
		memcpy(&code->code, snek_compile, snek_compile_size);
		code->size = snek_compile_size;
#ifdef DEBUG_COMPILE
		snek_code_dump(code);
#endif
	}
	snek_compile_size = 0;
	compile_alloc = 0;
	snek_compile = NULL;
	return code;
}

snek_offset_t
snek_code_line(snek_code_t *code)
{
	snek_offset_t	ip;
	snek_offset_t	line = 0;
	snek_op_t	op;

	for (ip = 0; ip < code->size; ip += snek_op_extra_size(op)) {
		op = code->code[ip++];
		if (op == snek_op_line) {
			memcpy(&line, &code->code[ip], sizeof (snek_offset_t));
			break;
		}
	}
	return line;
}

snek_poly_t	snek_stack[SNEK_STACK];
snek_offset_t	snek_stackp;
snek_poly_t 	snek_a;
snek_code_t	*snek_code;

float
snek_poly_get_float(snek_poly_t a)
{
	if (snek_poly_type(a) == snek_float)
		return snek_poly_to_float(a);
	snek_error("not a number: %p", a);
	return 0.0f;
}

snek_soffset_t
snek_poly_get_soffset(snek_poly_t a)
{
	return (snek_soffset_t) snek_poly_get_float(a);
}

void
snek_stack_push(snek_poly_t p)
{
	if (snek_stackp == SNEK_STACK) {
		snek_error("stack overflow");
		return;
	}
	snek_stack[snek_stackp++] = p;
}

snek_poly_t
snek_stack_pop(void)
{
#if SNEK_DEBUG
	if (!snek_stackp)
		snek_panic("stack underflow");
#endif
	return snek_stack[--snek_stackp];
}

snek_poly_t
snek_stack_pick(snek_offset_t off)
{
#if SNEK_DEBUG
	if (off >= snek_stackp)
		snek_panic("stack underflow");
#endif
	return snek_stack[snek_stackp - off - 1];
}

void
snek_stack_drop(snek_offset_t off)
{
#if SNEK_DEBUG
	if (off > snek_stackp)
		snek_panic("stack underflow");
#endif
	snek_stackp -= off;
}

float
snek_stack_pop_float(void)
{
	return snek_poly_get_float(snek_stack_pop());
}

snek_soffset_t
snek_stack_pop_soffset(void)
{
	return (snek_soffset_t) snek_stack_pop_float();
}

static void
snek_range_start(snek_offset_t ip)
{
	/* Fetch params from instruction */
	snek_offset_t	nactual;
	snek_id_t	id;
	uint8_t		for_depth;

	memcpy(&nactual, &snek_code->code[ip], sizeof(snek_offset_t));
	memcpy(&id, &snek_code->code[ip + sizeof(snek_offset_t) + sizeof (uint8_t)], sizeof (snek_id_t));
	memcpy(&for_depth, &snek_code->code[ip + sizeof(snek_offset_t)], sizeof (uint8_t));

	float current = 0.0f;
	float limit = 0.0f;
	float step = 1.0f;

	switch (nactual) {
	case 1:
		limit = snek_stack_pop_float();
		break;
	case 2:
		limit = snek_stack_pop_float();
		current = snek_stack_pop_float();
		break;
	case 3:
		step = snek_stack_pop_float();
		limit = snek_stack_pop_float();
		current = snek_stack_pop_soffset();
		if (step == 0) {
			snek_error("zero range step");
			return;
		}
		break;
	default:
		snek_error("invalid range: %d", nactual);
		snek_stack_drop(nactual);
		return;
	}

	/* Assign initial value (current - step) */

	snek_poly_t	*id_ref = snek_id_ref(id, true);
	if(!id_ref)
		return;
	*id_ref = snek_float_to_poly(current - step);

	/* Save limit */
	snek_poly_t	*limit_ref = snek_id_ref(snek_for_tmp(for_depth, 0), true);
	if (!limit_ref)
		return;
	*limit_ref = snek_float_to_poly(limit);

	/* Save step */
	snek_poly_t	*step_ref = snek_id_ref(snek_for_tmp(for_depth, 1), true);
	if (!step_ref)
		return;
	*step_ref = snek_float_to_poly(step);

	return;
}

static bool
snek_range_step(snek_offset_t ip)
{
	uint8_t		for_depth;
	snek_id_t	id;

	memcpy(&for_depth, &snek_code->code[ip + sizeof(snek_offset_t)], sizeof (uint8_t));
	memcpy(&id, &snek_code->code[ip + sizeof(snek_offset_t) + sizeof (uint8_t)], sizeof (snek_id_t));

	snek_poly_t	*id_ref = snek_id_ref(id, false);
	snek_poly_t	*limit_ref = snek_id_ref(snek_for_tmp(for_depth, 0), false);
	snek_poly_t	*step_ref = snek_id_ref(snek_for_tmp(for_depth, 1), false);

	if (!id_ref || !limit_ref || !step_ref)
		return false;

	float step = snek_poly_get_float(*step_ref);
	float value = snek_poly_get_float(*id_ref) + step;
	*id_ref = snek_float_to_poly(value);

	float limit = snek_poly_get_float(*limit_ref);
	if (step > 0 ? value >= limit : value <= limit)
		return false;
	return true;
}

static bool
snek_in_step(snek_offset_t ip)
{
	uint8_t	for_depth;
	snek_id_t id;
	snek_poly_t *ref;

	memcpy(&for_depth, &snek_code->code[ip + sizeof(snek_offset_t)], sizeof(uint8_t));

	/* Get current index, save next index */
	snek_poly_t *i_ref = snek_id_ref(snek_for_tmp(for_depth, 1), false);
	snek_soffset_t i = snek_poly_get_soffset(*i_ref);
	*i_ref = snek_float_to_poly(i + 1);

	/* Fetch iterable */
	snek_poly_t array = *snek_id_ref(snek_for_tmp(for_depth, 0), false);

	/* Compute current value */
	snek_poly_t value = SNEK_NULL;

	switch (snek_poly_type(array)) {
	case snek_list:
		value = snek_list_get(snek_poly_to_list(array), i, false);
		break;
	case snek_string:
		value = snek_string_get(snek_poly_to_string(array), i, false);
		break;
	default:
		snek_error("not iterable: %p", array);
		return true;
	}
	/* End of iteration */
	if (snek_is_null(value))
		return false;

	/* Update value */
	memcpy(&id, &snek_code->code[ip + sizeof(snek_offset_t) + sizeof (uint8_t)], sizeof (snek_id_t));

	snek_stack_push(value);
	ref = snek_id_ref(id, true);
	value = snek_stack_pop();
	if (!ref)
		return false;
	*ref = value;
	return true;
}

static snek_poly_t
snek_binary(snek_poly_t a, snek_op_t op, snek_poly_t b, bool inplace)
{
	snek_list_t	*al;
	snek_list_t	*bl;
	float		af;
	float		bf;
	bool		found;
	snek_poly_t	ret = SNEK_NULL;

	switch (op) {
	case snek_op_eq:
		return snek_bool_to_poly(snek_poly_equal(a, b, false));
	case snek_op_ne:
		return snek_bool_to_poly(!snek_poly_equal(a, b, false));
	case snek_op_is:
		return snek_bool_to_poly(snek_poly_equal(a, b, true));
	case snek_op_is_not:
		return snek_bool_to_poly(!snek_poly_equal(a, b, true));
	default:
		break;
	}

	snek_type_t	at = snek_poly_type(a);
	snek_type_t	bt = snek_poly_type(b);

	if (op == snek_op_array) {
		snek_soffset_t bo = snek_poly_get_soffset(b);

		switch (at) {
		case snek_list:
			ret = snek_list_get(snek_poly_to_list(a), bo, true);
			break;
		case snek_string:
			ret = snek_string_get(snek_poly_to_string(a), bo, true);
			break;
		default:
			break;
		}
	} else if (at == snek_float && bt == snek_float) {
		af = snek_poly_to_float(a);
		bf = snek_poly_to_float(b);
		switch (op) {
		case snek_op_lt:
			ret = snek_bool_to_poly(af < bf);
			break;
		case snek_op_gt:
			ret = snek_bool_to_poly(af > bf);
			break;
		case snek_op_le:
			ret = snek_bool_to_poly(af <= bf);
			break;
		case snek_op_ge:
			ret = snek_bool_to_poly(af >= bf);
			break;
		default:
			switch (op) {
			case snek_op_plus:
				af = af + bf;
				break;
			case snek_op_minus:
				af = af - bf;
				break;
			case snek_op_times:
				af = af * bf;
				break;
			case snek_op_divide:
				af = af / bf;
				break;
			case snek_op_div:
				af = (float) ((int32_t) af / (int32_t) bf);
				break;
			case snek_op_mod:
				af = (float) ((int32_t) af % (int32_t) bf);
				break;
			case snek_op_pow:
				af = powf(af, bf);
				break;
			case snek_op_land:
				af = (float) ((int32_t) af & (int32_t) bf);
				break;
			case snek_op_lor:
				af = (float) ((int32_t) af | (int32_t) bf);
				break;
			case snek_op_lxor:
				af = (float) ((int32_t) af ^ (int32_t) bf);
				break;
			case snek_op_lshift:
				af = (float) ((int32_t) af << (int32_t) bf);
				break;
			case snek_op_rshift:
				af = (float) ((int32_t) af >> (int32_t) bf);
				break;
			default:
				break;
			}
			ret = snek_float_to_poly(af);
		}
	} else {
		switch (op) {
		case snek_op_in:
		case snek_op_not_in:
			if (bt == snek_list) {
				snek_offset_t	o;
				bl = snek_poly_to_list(b);
				found = false;
				for (o = 0; o < bl->size; o++) {
					if (snek_poly_equal(a, snek_list_data(bl)[o], false)) {
						found = true;
						break;
					}
				}
				ret = snek_bool_to_poly(found == (op == snek_op_in));
			}
			if (at == snek_string && bt == snek_string) {
				found = strstr(snek_poly_to_string(b), snek_poly_to_string(a)) != NULL;
				ret = snek_bool_to_poly(found == (op == snek_op_in));
			}
			break;
		case snek_op_plus:
			if (at == snek_string && bt == snek_string) {
				ret = snek_string_cat(snek_poly_to_string(a),
						      snek_poly_to_string(b));
			} else if (at == snek_list && bt == snek_list) {
				al = snek_poly_to_list(a);
				bl = snek_poly_to_list(b);

				if (snek_list_readonly(al) == snek_list_readonly(bl)) {
					if (inplace && !snek_list_readonly(al))
						al = snek_list_append(al, bl);
					else
						al = snek_list_plus(al, bl);
					ret = snek_list_to_poly(al);
				}
			}
			break;
		case snek_op_times:
			if (at == snek_list && bt == snek_float) {
				al = snek_poly_to_list(a);
				snek_soffset_t bo = snek_poly_get_soffset(b);
				ret = snek_list_to_poly(snek_list_times(al, bo));
			}
			break;
		case snek_op_mod:
			if (at == snek_string)
				ret = snek_string_to_poly(snek_string_interpolate(snek_poly_to_string(a), b));
			break;
		default:
			break;
		}
	}
	if (snek_is_null(ret)) {
		snek_error("type mismatch: %p %p", a, b);
		return SNEK_ZERO;
	}
	return ret;
}

static bool
snek_slice_canon(snek_slice_t *slice)
{
	if (slice->start == SNEK_SLICE_DEFAULT)
		slice->start = 0;
	if (slice->stride == SNEK_SLICE_DEFAULT)
		slice->stride = 1;
	if (slice->end == SNEK_SLICE_DEFAULT) {
		if (slice->stride < 0)
			slice->end = -slice->len - 1;
		else
			slice->end = slice->len;
	}
	if (slice->start < 0) {
		slice->start = slice->len + slice->start;
		if (slice->start < 0)
			slice->start = 0;
	}
	if (slice->end < 0) {
		slice->end = slice->len + slice->end;
	}
	if (slice->start > slice->len)
		return false;
	if (slice->end > slice->len)
		return false;
	if (slice->stride == 0)
		return false;
	slice->count = (slice->end - slice->start) / abs(slice->stride);
	return true;
}

static void
snek_slice(uint8_t bits)
{
	snek_slice_t	slice = {
		.start = SNEK_SLICE_DEFAULT,
		.end = SNEK_SLICE_DEFAULT,
		.stride = SNEK_SLICE_DEFAULT,

		.len = 0,
		.count = 0,
		.pos = 0,
	};

	if (bits & SNEK_OP_SLICE_STRIDE)
		slice.stride = snek_stack_pop_soffset();

	if (bits & SNEK_OP_SLICE_END)
		slice.end = snek_stack_pop_soffset();

	if (bits & SNEK_OP_SLICE_START)
		slice.start = snek_stack_pop_soffset();

	slice.len = snek_poly_len(snek_a = snek_stack_pop());

	if (!snek_slice_canon(&slice))
		return;
	switch (snek_poly_type(snek_a)) {
	case snek_string:
		snek_a = snek_string_to_poly(snek_string_slice(snek_poly_to_string(snek_a), &slice));
		break;
	case snek_list:
		snek_a = snek_list_to_poly(snek_list_slice(snek_poly_to_list(snek_a), &slice));
		break;
	default:
		break;
	}
}

void
snek_undefined(snek_id_t id)
{
	snek_error("undefined: %s", snek_name_string(id));
}

static void
snek_assign(snek_id_t id, snek_op_t op)
{
	snek_poly_t *ref;

	if (op == snek_op_assign_named) {
		if (snek_id_is_local(id))
			return;
		op = snek_op_assign;
	}
	for (;;) {
		if (id != SNEK_ID_NONE) {
			ref = snek_id_ref(id, true);
			if (!ref) {
				snek_undefined(id);
				return;
			}
		} else {
			snek_poly_t ip = snek_stack_pop();
			snek_poly_t lp = snek_stack_pop();

			snek_list_t	*l;

			if (snek_poly_type(lp) != snek_list ||
			    snek_list_readonly(l = snek_poly_to_list(lp)))
			{
				snek_error("not a list: %p", lp);
				return;
			}

			snek_soffset_t	o = snek_poly_get_soffset(ip);
			ref = snek_list_ref(l, o, true);
			if (!ref)
				return;
		}

		if (op == snek_op_assign)
			break;

		/* Recover the two values popped from the stack so
		 * that they will be popped again
		 */
		if (id == SNEK_ID_NONE)
			snek_stackp += 2;
		snek_a = snek_binary(*ref, op - (snek_op_assign_plus - snek_op_plus), snek_a, true);
		op = snek_op_assign;
	}
	*ref = snek_a;
}

static void
snek_call_builtin(const snek_builtin_t *builtin, uint8_t nposition, uint8_t nnamed)
{
	snek_poly_t *actuals = &snek_stack[snek_stackp - (nposition + (nnamed << 1))];
	snek_soffset_t nformal = SNEK_BUILTIN_NFORMAL(builtin);

	if (nformal < 0) {
		snek_a = SNEK_BUILTIN_FUNCV(builtin)(nposition, nnamed, actuals);
	} else if (nposition != nformal || nnamed) {
		snek_error("wrong number of args: wanted %d, got %d", nformal, nposition);
	} else {
		switch (nformal) {
		case 0:
			snek_a = SNEK_BUILTIN_FUNC0(builtin)();
			break;
		case 1:
			snek_a = SNEK_BUILTIN_FUNC1(builtin)(actuals[0]);
			break;
#if SNEK_BUILTIN_NAMES_MAX_ARGS >= 2
		case 2:
			snek_a = SNEK_BUILTIN_FUNC2(builtin)(actuals[0], actuals[1]);
			break;
#endif
#if SNEK_BUILTIN_NAMES_MAX_ARGS >= 3
		case 3:
			snek_a = SNEK_BUILTIN_FUNC3(builtin)(actuals[0], actuals[1], actuals[2]);
			break;
#endif
		}
	}
}

static void
snek_run_do(bool (*poly)(snek_poly_t *p))
{
	snek_offset_t s;
	for (s = 0; s < snek_stackp; s++)
		poly(&snek_stack[s]);
}

void
snek_run_mark(void)
{
	snek_run_do(snek_poly_mark_ref);
}

void
snek_run_move(void)
{
	snek_run_do(snek_poly_move);
}

snek_poly_t
snek_code_run(snek_code_t *code_in)
{
	snek_code = code_in;

	snek_poly_t	*ref = NULL;
	snek_id_t	id;
	snek_offset_t	ip = 0;
	snek_offset_t	o;

	while (snek_code) {
		while (ip < snek_code->size) {
#ifdef DEBUG_EXEC
			snek_code_dump_instruction(snek_code, ip);
#endif
			snek_op_t op = snek_code->code[ip++];
			bool push = (op & snek_op_push) != 0;
			op &= ~snek_op_push;
			switch(op) {
			case snek_op_eq:
			case snek_op_ne:
			case snek_op_gt:
			case snek_op_lt:
			case snek_op_ge:
			case snek_op_le:

			case snek_op_is:
			case snek_op_is_not:
			case snek_op_in:
			case snek_op_not_in:

			case snek_op_array:

			case snek_op_plus:
			case snek_op_minus:
			case snek_op_times:
			case snek_op_divide:
			case snek_op_div:
			case snek_op_mod:
			case snek_op_pow:
			case snek_op_land:
			case snek_op_lor:
			case snek_op_lxor:
			case snek_op_lshift:
			case snek_op_rshift:
				snek_a = snek_binary(snek_stack_pick(0), op, snek_a, false);
				snek_stack_drop(1);
				break;

			case snek_op_assign_plus:
			case snek_op_assign_minus:
			case snek_op_assign_times:
			case snek_op_assign_divide:
			case snek_op_assign_div:
			case snek_op_assign_mod:
			case snek_op_assign_pow:
			case snek_op_assign_land:
			case snek_op_assign_lor:
			case snek_op_assign_lxor:
			case snek_op_assign_lshift:
			case snek_op_assign_rshift:

			case snek_op_assign:
			case snek_op_assign_named:
				memcpy(&id, &snek_code->code[ip], sizeof (snek_id_t));
				ip += sizeof (snek_id_t);
				snek_assign(id, op);
				break;

			case snek_op_num:
				memcpy(&snek_a.f, &snek_code->code[ip], sizeof(float));
				ip += sizeof(float);
				break;
			case snek_op_int:
				snek_a.f = (int8_t) snek_code->code[ip];
				ip += 1;
				break;
			case snek_op_string:
				memcpy(&o, &snek_code->code[ip], sizeof(snek_offset_t));
				ip += sizeof (snek_offset_t);
				snek_a = snek_offset_to_poly(o, snek_string);
				break;
			case snek_op_list:
			case snek_op_tuple:
				memcpy(&o, &snek_code->code[ip], sizeof(snek_offset_t));
				ip += sizeof (snek_offset_t);
				snek_a = snek_list_imm(o, op == snek_op_tuple);
				break;
			case snek_op_id:
				memcpy(&id, &snek_code->code[ip], sizeof(snek_id_t));
				ip += sizeof (snek_id_t);
				ref = snek_id_ref(id, false);
				if (ref) {
					snek_a = *ref;
					break;
				}
				if (id < SNEK_BUILTIN_END) {
					snek_a = snek_builtin_id_to_poly(id);
					break;
				}
				snek_undefined(id);
				break;
			case snek_op_not:
				snek_a = snek_bool_to_poly(!snek_poly_true(snek_a));
				break;
			case snek_op_uminus:
				snek_a = snek_float_to_poly(-snek_poly_get_float(snek_a));
				break;
			case snek_op_lnot:
				snek_a = snek_float_to_poly(~(uint32_t) snek_poly_get_float(snek_a));
				break;
			case snek_op_call:
				memcpy(&o, &snek_code->code[ip], sizeof (snek_offset_t));
				snek_offset_t nposition = (o & 0xff);
				snek_offset_t nnamed = (o >> 8);
				snek_offset_t nstack = nposition + (nnamed<<1);
				snek_a = snek_stack_pick(nstack);
				switch (snek_poly_type(snek_a)) {
				case snek_func:
					if (!snek_func_push(nposition, nnamed, ip - 1))
						break;
					snek_a = snek_stack_pop();	/* get function back */
					snek_code = snek_pool_ref(snek_poly_to_func(snek_a)->code);
					ip = 0;
					push = false;	/* will pick up push on return */
					goto done_func;
				case snek_builtin:
					snek_call_builtin(snek_poly_to_builtin(snek_a), nposition, nnamed);
					break;
				default:
					snek_error("not a func: %p", snek_a);
					break;
				}
				ip += sizeof (snek_offset_t);
				snek_stack_drop(nstack + 1);
			done_func:
				break;
			case snek_op_slice:
				snek_slice(snek_code->code[ip]);
				ip++;
				break;
			case snek_op_global:
				memcpy(&id, &snek_code->code[ip], sizeof (snek_id_t));
				ip += sizeof (snek_id_t);
				snek_frame_mark_global(id);
				break;
			case snek_op_branch:
				memcpy(&ip, &snek_code->code[ip], sizeof (snek_offset_t));
				break;
			case snek_op_branch_true:
				if (snek_poly_true(snek_a))
					memcpy(&ip, &snek_code->code[ip], sizeof (snek_offset_t));
				else
					ip += sizeof (snek_offset_t);
				break;
			case snek_op_branch_false:
				if (!snek_poly_true(snek_a))
					memcpy(&ip, &snek_code->code[ip], sizeof (snek_offset_t));
				else
					ip += sizeof (snek_offset_t);
				break;
			case snek_op_forward:
				snek_error("not in loop");
				break;
			case snek_op_range_start:
				snek_range_start(ip);
				ip += sizeof (snek_offset_t) + sizeof (uint8_t) + sizeof(snek_id_t);
				break;
			case snek_op_range_step:
				if (!snek_range_step(ip))
					memcpy(&ip, &snek_code->code[ip], sizeof (snek_offset_t));
				else
					ip += sizeof (snek_offset_t) + sizeof (uint8_t) + sizeof(snek_id_t);
				break;
			case snek_op_in_step:
				if (!snek_in_step(ip))
					memcpy(&ip, &snek_code->code[ip], sizeof (snek_offset_t));
				else
					ip += sizeof (snek_offset_t) + sizeof (uint8_t) + sizeof (snek_id_t);
				break;
			case snek_op_line:
				memcpy(&o, &snek_code->code[ip], sizeof (snek_offset_t));
				ip += sizeof (snek_offset_t);
				snek_line = o;
				break;
			case snek_op_nop:
			case snek_op_push:
				break;
			}
			if (snek_abort)
				goto abort;
			if (push)
				snek_stack_push(snek_a);
#ifdef DEBUG_EXEC
			dbg("\t\ta= "); snek_poly_print(stddbg, snek_a, 'r');
			for (o = snek_stackp; o;) {
				dbg(", [%d]= ", snek_stackp - o);
				snek_poly_print(stddbg, snek_stack[--o], 'r');
			}
			dbg("\n");
#endif
		}
		ip = snek_frame_pop();
		if (snek_code) {
			snek_op_t op = snek_code->code[ip];
			ip += sizeof (snek_offset_t) + 1;
			if ((op & snek_op_push) != 0)
				snek_stack_push(snek_a);
		}
	}
abort:
	/* Clear references to run objects */
	snek_code = NULL;
	snek_frame = NULL;
	snek_stackp = 0;
	snek_poly_t ret = snek_a;
	snek_a = SNEK_ZERO;
	return ret;
}

static snek_offset_t
snek_code_size(void *addr)
{
	snek_code_t *code = addr;

	return (snek_offset_t) sizeof (snek_code_t) + code->size;
}

static void
code_mark(uint8_t *code, snek_offset_t size)
{
	snek_offset_t ip = 0;

	while (ip < size) {
		snek_op_t op = code[ip++] & ~snek_op_push;
		snek_offset_t o;
		switch (op) {
		case snek_op_string:
			memcpy(&o, &code[ip], sizeof (snek_offset_t));
			snek_mark_offset(&snek_mems[snek_string], o);
			break;
		default:
			break;
		}
		ip += snek_op_extra_size(op);
	}
}

static void
code_move(uint8_t *code, snek_offset_t size)
{
	snek_offset_t	ip = 0;

	while (ip < size) {
		snek_op_t op = code[ip++] & ~snek_op_push;
		switch (op) {
		case snek_op_string:
			snek_move_block_offset(&code[ip]);
			break;
		default:
			break;
		}
		ip += snek_op_extra_size(op);
	}
}

static void
snek_code_mark(void *addr)
{
	snek_code_t	*code = addr;

	code_mark(code->code, code->size);
}

static void
snek_code_move(void *addr)
{
	snek_code_t	*code = addr;

	code_move(code->code, code->size);
}

const snek_mem_t SNEK_MEM_DECLARE(snek_code_mem) = {
	.size = snek_code_size,
	.mark = snek_code_mark,
	.move = snek_code_move,
	SNEK_MEM_DECLARE_NAME("code")
};

static snek_offset_t
_snek_compile_size(void *addr)
{
	(void) addr;
	return compile_alloc;
}

static void
snek_compile_mark(void *addr)
{
	code_mark(addr, snek_compile_size);
}

static void
snek_compile_move(void *addr)
{
	code_move(addr, snek_compile_size);
}

const snek_mem_t SNEK_MEM_DECLARE(snek_compile_mem) = {
	.size = _snek_compile_size,
	.mark = snek_compile_mark,
	.move = snek_compile_move,
	SNEK_MEM_DECLARE_NAME("compile")
};
