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

static int
newt_op_extra_size(newt_op_t op)
{
	switch (op) {
	case newt_op_num:
		return sizeof (float);
	case newt_op_string:
		return sizeof (newt_offset_t);
	case newt_op_list:
	case newt_op_tuple:
		return sizeof (newt_offset_t);
	case newt_op_id:
	case newt_op_global:
	case newt_op_assign:
	case newt_op_assign_plus:
	case newt_op_assign_minus:
	case newt_op_assign_times:
	case newt_op_assign_divide:
	case newt_op_assign_div:
	case newt_op_assign_mod:
	case newt_op_assign_pow:
	case newt_op_assign_land:
	case newt_op_assign_lor:
	case newt_op_assign_lxor:
	case newt_op_assign_lshift:
	case newt_op_assign_rshift:
	case newt_op_in_start:
		return sizeof (newt_id_t);
	case newt_op_call:
		return sizeof (newt_offset_t);
	case newt_op_slice:
		return 1;
	case newt_op_branch:
	case newt_op_branch_true:
	case newt_op_branch_false:
	case newt_op_forward:
	case newt_op_range_step:
	case newt_op_in_step:
	case newt_op_line:
		return sizeof (newt_offset_t);
	case newt_op_range_start:
		return sizeof (newt_id_t) + sizeof (newt_offset_t);
	default:
		return 0;
	}
}

//#define DEBUG_COMPILE
//#define DEBUG_EXEC
#if defined(DEBUG_COMPILE) || defined(DEBUG_EXEC)

const char * const newt_op_names[] = {
	[newt_op_plus] = "plus",
	[newt_op_minus] = "minus",
	[newt_op_times] = "times",
	[newt_op_divide] = "divide",
	[newt_op_div] = "div",
	[newt_op_mod] = "mod",
	[newt_op_pow] = "pow",
	[newt_op_land] = "land",
	[newt_op_lor] = "lor",
	[newt_op_lxor] = "lxor",
	[newt_op_lshift] = "lshift",
	[newt_op_rshift] = "rshift",

	[newt_op_assign_plus] = "assign_plus",
	[newt_op_assign_minus] = "assign_minus",
	[newt_op_assign_times] = "assign_times",
	[newt_op_assign_divide] = "assign_divide",
	[newt_op_assign_div] = "assign_div",
	[newt_op_assign_mod] = "assign_mod",
	[newt_op_assign_pow] = "assign_pow",
	[newt_op_assign_land] = "assign_land",
	[newt_op_assign_lor] = "assign_lor",
	[newt_op_assign_lxor] = "assign_lxor",
	[newt_op_assign_lshift] = "assign_lshift",
	[newt_op_assign_rshift] = "assign_rshift",

	[newt_op_num] = "num",
	[newt_op_string] = "string",
	[newt_op_list] = "list",
	[newt_op_tuple] = "tuple",
	[newt_op_id] = "id",


	[newt_op_not] = "not",

	[newt_op_eq] = "eq",
	[newt_op_ne] = "ne",
	[newt_op_gt] = "gt",
	[newt_op_lt] = "lt",
	[newt_op_ge] = "ge",
	[newt_op_le] = "le",

	[newt_op_is] = "is",
	[newt_op_is_not] = "is_not",
	[newt_op_in] = "in",
	[newt_op_not_in] = "not_in",

	[newt_op_uminus] = "uminus",
	[newt_op_lnot] = "lnot",

	[newt_op_call] = "call",

	[newt_op_array] = "array",
	[newt_op_slice] = "slice",

	[newt_op_assign] = "assign",

	[newt_op_global] = "global",

	[newt_op_branch] = "branch",
	[newt_op_branch_true] = "branch_true",
	[newt_op_branch_false] = "branch_false",
	[newt_op_forward] = "forward",
	[newt_op_range_start] = "range_start",
	[newt_op_range_step] = "range_step",
	[newt_op_in_start] = "in_start",
	[newt_op_in_step] = "in_step",
	[newt_op_line] = "line",
};

newt_offset_t
newt_code_dump_instruction(newt_code_t *code, newt_offset_t ip)
{
	float		f;
	newt_id_t	id;
	newt_offset_t	o;

	printf("%6d:  ", ip);
	newt_op_t op = code->code[ip++];
	bool push = (op & newt_op_push) != 0;
	op &= ~newt_op_push;
	printf("%-12s %c ", newt_op_names[op], push ? '^' : ' ');
	switch(op) {
	case newt_op_num:
		memcpy(&f, &code->code[ip], sizeof(float));
		printf("%g\n", f);
		break;
	case newt_op_string:
		memcpy(&o, &code->code[ip], sizeof(newt_offset_t));
		printf("%s\n", (char *) newt_pool_ref(o));
		break;
	case newt_op_list:
	case newt_op_tuple:
		memcpy(&o, &code->code[ip], sizeof(newt_offset_t));
		printf("%u\n", o);
		break;
	case newt_op_id:
	case newt_op_global:
	case newt_op_assign:
	case newt_op_assign_plus:
	case newt_op_assign_minus:
	case newt_op_assign_times:
	case newt_op_assign_divide:
	case newt_op_assign_div:
	case newt_op_assign_mod:
	case newt_op_assign_pow:
	case newt_op_assign_land:
	case newt_op_assign_lor:
	case newt_op_assign_lxor:
	case newt_op_assign_lshift:
	case newt_op_assign_rshift:
	case newt_op_in_start:
		memcpy(&id, &code->code[ip], sizeof(newt_id_t));
		printf("(%5d) ", id);
		if (id)
			printf("%s\n", newt_name_string(id));
		else
			printf("<array>\n");
		break;
	case newt_op_call:
		memcpy(&o, &code->code[ip], sizeof(newt_offset_t));
		printf("%d actuals\n", o);
		break;
	case newt_op_slice:
		if (code->code[ip] & NEWT_OP_SLICE_START) printf(" start");
		if (code->code[ip] & NEWT_OP_SLICE_END) printf(" end");
		if (code->code[ip] & NEWT_OP_SLICE_STRIDE) printf(" stride");
		break;
	case newt_op_branch:
	case newt_op_branch_true:
	case newt_op_branch_false:
	case newt_op_forward:
	case newt_op_range_step:
	case newt_op_in_step:
	case newt_op_line:
		memcpy(&o, &code->code[ip], sizeof (newt_offset_t));
		printf("%d\n", o);
		break;
	case newt_op_range_start:
		memcpy(&id, &code->code[ip], sizeof (newt_id_t));
		if (id)
			printf("%s", newt_name_string(id));
		else
			printf("<array>");
		memcpy(&o, &code->code[ip + sizeof (newt_id_t)], sizeof (newt_offset_t));
		printf(", %d\n", o);
		break;
	default:
		printf("\n");
		break;
	}
	return ip + newt_op_extra_size(op);
}

void
newt_code_dump(newt_code_t *code)
{
	newt_offset_t	ip = 0;

	while (ip < code->size) {
		ip = newt_code_dump_instruction(code, ip);
	}
}

#endif

uint8_t			*newt_compile;
static newt_offset_t	compile_size, compile_alloc;
static newt_offset_t	compile_prev, compile_prev_prev;

#define COMPILE_INC	32

static newt_offset_t
compile_extend(newt_offset_t n, void *data, bool insn)
{
	newt_offset_t start = compile_size;

	if (compile_size + n > compile_alloc) {
		uint8_t *new_compile = newt_alloc(compile_alloc + COMPILE_INC);
		if (!new_compile)
			return 0xffff;
		memcpy(new_compile, newt_compile, compile_size);
		compile_alloc += COMPILE_INC;
		newt_compile = new_compile;
	}
	if (data)
		memcpy(newt_compile + compile_size, data, n);
	if (insn) {
		compile_prev_prev = compile_prev;
		compile_prev = compile_size;
	}
	compile_size += n;
	return start;
}

newt_offset_t
newt_code_current(void)
{
	return compile_size;
}

newt_offset_t
newt_code_prev_insn(void)
{
	return compile_prev;
}

newt_offset_t
newt_code_prev_prev_insn(void)
{
	return compile_prev_prev;
}

uint8_t *
newt_code_at(newt_offset_t offset)
{
	return newt_compile + offset;
}

void
newt_code_delete_prev(void)
{
	compile_size = compile_prev;
	compile_prev = compile_prev_prev;
}

newt_offset_t
newt_code_add_op(newt_op_t op)
{
	return compile_extend(1, &op, true);
}

newt_offset_t
newt_code_add_op_id(newt_op_t op, newt_id_t id)
{
	newt_offset_t offset = compile_extend(1, &op, true);
	compile_extend(sizeof (newt_id_t), &id, false);
	return offset;
}

newt_offset_t
newt_code_add_number(float number)
{
	newt_op_t op = newt_op_num;
	newt_offset_t offset;

	offset = compile_extend(1, &op, true);
	compile_extend(sizeof(float), &number, false);
	return offset;
}

newt_offset_t
newt_code_add_string(char *string)
{
	newt_op_t op = newt_op_string;
	newt_offset_t offset, strpos;
	newt_offset_t s;

	newt_poly_stash(newt_string_to_poly(string));
	offset = compile_extend(1, &op, true);
	strpos = compile_extend(sizeof (newt_offset_t), NULL, false);
	string = newt_poly_to_string(newt_poly_fetch());
	s = newt_pool_offset(string);
	memcpy(newt_compile + strpos, &s, sizeof (newt_offset_t));
	return offset;
}

newt_offset_t
newt_code_add_op_offset(newt_op_t op, newt_offset_t o)
{
	newt_offset_t offset;

	offset = compile_extend(1, &op, true);
	compile_extend(sizeof (newt_offset_t), &o, false);
	return offset;
}

newt_offset_t
newt_code_add_forward(newt_forward_t forward)
{
	newt_op_t op = newt_op_forward;
	newt_offset_t offset;

	offset = compile_extend(1, &op, true);
	compile_extend(sizeof (newt_forward_t), &forward, false);
	if (sizeof (newt_forward_t) < sizeof (newt_offset_t))
		compile_extend(sizeof (newt_offset_t) - sizeof (newt_forward_t), NULL, false);
	return offset;
}

newt_offset_t
newt_code_add_call(newt_offset_t nactual)
{
	newt_offset_t	offset;
	newt_op_t	op = newt_op_call;

	offset = compile_extend(1, &op, true);
	compile_extend(sizeof (newt_offset_t), &nactual, false);
	return offset;
}

static inline uint8_t
bit(bool val, uint8_t pos)
{
	return val ? pos : 0;
}

newt_offset_t
newt_code_add_slice(bool has_start, bool has_end, bool has_stride)
{
	uint8_t	insn[2];
	insn[0] = newt_op_slice;
	insn[1] = (bit(has_start, NEWT_OP_SLICE_START) |
		   bit(has_end,   NEWT_OP_SLICE_END) |
		   bit(has_stride, NEWT_OP_SLICE_STRIDE));
	return compile_extend(2, insn, true);
}

newt_offset_t
newt_code_add_range_start(newt_id_t id, newt_offset_t nactual)
{
	newt_op_t op = newt_op_range_start;
	newt_offset_t offset;

	offset = compile_extend(1, &op, true);
	compile_extend(sizeof (newt_id_t), &id, false);
	compile_extend(sizeof (newt_offset_t), &nactual, false);
	return offset;
}

void
newt_code_patch_branch(newt_offset_t branch, newt_offset_t target)
{
	memcpy(newt_compile + branch + 1, &target, sizeof (newt_offset_t));
}

void
newt_code_patch_forward(newt_offset_t start, newt_forward_t forward, newt_offset_t target)
{
	newt_offset_t ip = start;

	while (ip < compile_size) {
		newt_op_t op = newt_compile[ip++];
		bool push = (op & newt_op_push) != 0;
		newt_forward_t f;
		op &= ~newt_op_push;
		switch (op) {
		case newt_op_forward:
			memcpy(&f, &newt_compile[ip], sizeof (newt_forward_t));
			if (f == forward) {
				newt_compile[ip-1] = newt_op_branch | (push ? newt_op_push : 0);
				memcpy(&newt_compile[ip], &target, sizeof(newt_offset_t));
			}
			break;
		default:
			break;
		}
		ip += newt_op_extra_size(op);
	}
}

void
newt_code_set_push(newt_offset_t offset)
{
	newt_compile[offset] |= newt_op_push;
}

newt_code_t *
newt_code_finish(void)
{
	newt_code_patch_forward(0, newt_forward_return, newt_code_current());
	newt_code_t *code = newt_alloc(sizeof (newt_code_t) + compile_size);

	if (code) {
		memcpy(&code->code, newt_compile, compile_size);
		code->size = compile_size;
#ifdef DEBUG_COMPILE
		newt_code_dump(code);
#endif
	}
	compile_size = 0;
	compile_alloc = 0;
	newt_compile = NULL;
	return code;
}

newt_poly_t	newt_stack[NEWT_STACK];
newt_offset_t	newt_stackp = 0;
static newt_poly_t 	a = NEWT_ZERO, b = NEWT_ZERO;
static newt_code_t	*code;

static newt_poly_t
newt_binary(newt_poly_t a, newt_op_t op, newt_poly_t b, bool inplace)
{
	char		*as;
	char		*bs;
	newt_list_t	*al;
	newt_list_t	*bl;
	float		af;
	float		bf;
	int		i;
	bool		found;
	newt_poly_t	ret = NEWT_NULL;

	switch (op) {
	case newt_op_eq:
		return newt_bool_to_poly(newt_poly_equal(a, b));
	case newt_op_ne:
		return newt_bool_to_poly(!newt_poly_equal(a, b));
	case newt_op_is:
		return newt_bool_to_poly(a.u == b.u);
	case newt_op_is_not:
		return newt_bool_to_poly(a.u != b.u);
	default:
		break;
	}
	if (op == newt_op_array) {
		if (newt_is_float(b)) {
			bf = newt_poly_to_float(b);
			newt_offset_t bo = (newt_offset_t) bf;

			switch (newt_poly_type(a)) {
			case newt_list:
				al = newt_poly_to_list(a);
				if (bo < al->size)
					ret = newt_list_data(al)[bo];
				break;
			case newt_string:
				as = newt_poly_to_string(a);
				ret = newt_string_to_poly(newt_string_make(newt_string_fetch(as, bo)));
				break;
			default:
				break;
			}
		}
	} else if (newt_is_float(a) && newt_is_float(b)) {
		af = newt_poly_to_float(a);
		bf = newt_poly_to_float(b);
		switch (op) {
		case newt_op_lt:
			ret = newt_bool_to_poly(af < bf);
			break;
		case newt_op_gt:
			ret = newt_bool_to_poly(af > bf);
			break;
		case newt_op_le:
			ret = newt_bool_to_poly(af <= bf);
			break;
		case newt_op_ge:
			ret = newt_bool_to_poly(af >= bf);
			break;
		default:
			switch (op) {
			case newt_op_plus:
				af = af + bf;
				break;
			case newt_op_minus:
				af = af - bf;
				break;
			case newt_op_times:
				af = af * bf;
				break;
			case newt_op_divide:
				af = af / bf;
				break;
			case newt_op_div:
				af = (float) ((int32_t) af / (int32_t) bf);
				break;
			case newt_op_mod:
				af = (float) ((int32_t) af % (int32_t) bf);
				break;
			case newt_op_pow:
				af = powf(af, bf);
				break;
			case newt_op_land:
				af = (float) ((int32_t) af & (int32_t) bf);
				break;
			case newt_op_lor:
				af = (float) ((int32_t) af | (int32_t) bf);
				break;
			case newt_op_lxor:
				af = (float) ((int32_t) af ^ (int32_t) bf);
				break;
			default:
				break;
			}
			ret = newt_float_to_poly(af);
		}
	} else {
		switch (op) {
		case newt_op_in:
		case newt_op_not_in:
			if (newt_poly_type(b) == newt_list) {
				bl = newt_poly_to_list(b);
				found = false;
				for (i = 0; i < bl->size; i++) {
					if (newt_poly_equal(a, newt_list_data(bl)[i])) {
						found = true;
						break;
					}
				}
				ret = newt_bool_to_poly(found == (op == newt_op_in));
			}
			if (newt_poly_type(a) == newt_string && newt_poly_type(b) == newt_string) {
				found = strstr(newt_poly_to_string(b), newt_poly_to_string(a)) != NULL;
				ret = newt_bool_to_poly(found == (op == newt_op_in));
			}
			break;
		case newt_op_plus:
			if (newt_poly_type(a) == newt_string && newt_poly_type(b) == newt_string) {
				as = newt_poly_to_string(a);
				bs = newt_poly_to_string(b);
				ret = newt_string_to_poly(newt_string_cat(as, bs));
			} else if (newt_poly_type(a) == newt_list && newt_poly_type(b) == newt_list) {
				al = newt_poly_to_list(a);
				bl = newt_poly_to_list(b);

				if (newt_list_readonly(al) != newt_list_readonly(bl)) {
					newt_error("can't mix tuple with list");
				} else {
					if (inplace && !newt_list_readonly(al))
						al = newt_list_append(al, bl);
					else
						al = newt_list_plus(al, bl);
					if (!al)
						return NEWT_ZERO;
					ret = newt_list_to_poly(al);
				}
			}
			break;
		case newt_op_mod:
			if (newt_poly_type(a) == newt_string) {
				as = newt_poly_to_string(a);
				as = newt_string_interpolate(as, b);
				if (as)
					ret = newt_string_to_poly(as);
			}
			break;
		default:
			break;
		}
	}
	if (newt_is_null(ret)) {
		newt_error("type mismatch: %p %p", a, b);
		return NEWT_ZERO;
	}
	return ret;
}

static newt_poly_t
newt_unary(newt_op_t op, newt_poly_t a)
{
	switch (op) {
	case newt_op_not:
		return newt_bool_to_poly(!newt_poly_true(a));
	default:
		break;
	}
	if (newt_is_float(a)) {
		float af = newt_poly_to_float(a);
		switch (op) {
		case newt_op_uminus:
			af = -af;
			break;
		default:
			break;
		}
		a = newt_float_to_poly(af);
	}
	return a;
}

static newt_soffset_t
newt_poly_to_soffset(newt_poly_t a)
{
	if (newt_is_float(a))
		return (newt_soffset_t) newt_poly_to_float(a);
	return 0;
}

static newt_poly_t
newt_slice(newt_poly_t a, uint8_t bits)
{
	newt_slice_t	slice = {
		.start = NEWT_SLICE_DEFAULT,
		.end = NEWT_SLICE_DEFAULT,
		.stride = NEWT_SLICE_DEFAULT,

		.len = 0,
		.count = 0,
		.pos = 0,
	};
	bool used_a = false;

	if (bits & NEWT_OP_SLICE_STRIDE) {
		slice.stride = newt_poly_to_soffset(a);
		used_a = true;
	}

	if (bits & NEWT_OP_SLICE_END) {
		slice.end = newt_poly_to_soffset(used_a ? newt_stack_pop() : a);
		used_a = true;
	}

	if (bits & NEWT_OP_SLICE_START) {
		slice.start = newt_poly_to_soffset(used_a ? newt_stack_pop() : a);
		used_a = true;
	}

	if (used_a)
		a = newt_stack_pop();

	slice.len = newt_poly_len(a);

	if (!newt_slice_canon(&slice))
		return a;

	switch (newt_poly_type(a)) {
	case newt_string:
		a = newt_string_to_poly(newt_string_slice(newt_poly_to_string(a), &slice));
		break;
	case newt_list:
		a = newt_list_to_poly(newt_list_slice(newt_poly_to_list(a), &slice));
		break;
	default:
		break;
	}
	return a;
}

static void
newt_assign(newt_id_t id, newt_op_t op)
{
	newt_poly_t *ref;

	for (;;) {
		if (id != NEWT_ID_NONE) {
			ref = newt_id_ref(id, true);
			if (!ref) {
				newt_error("undefined \"%i\"", id);
				return;
			}
		} else {
			newt_poly_t ip = newt_stack_pop();
			newt_poly_t lp = newt_stack_pop();

			if (newt_poly_type(lp) != newt_list) {
				newt_error("not a list: %p", lp);
				return;
			}
			if (newt_poly_type(ip) != newt_float) {
				newt_error("not a number: %p", ip);
				return;
			}
			newt_list_t	*l = newt_poly_to_list(lp);
			float		f = newt_poly_to_float(ip);

			if (newt_list_readonly(l)) {
				newt_error("cannot assign to tuple");
				return;
			}
			if (f < 0 || l->size <= f) {
				newt_error("list index out of range: %p", ip);
				return;
			}
			ref = &newt_list_data(l)[(newt_offset_t) f];
		}

		if (op == newt_op_assign)
			break;

		/* Recover the two values popped from the stack so
		 * that they will be popped again
		 */
		if (id != NEWT_ID_NONE)
			newt_stackp += 2;
		a = newt_binary(*ref, op - (newt_op_assign_plus - newt_op_plus), a, true);
		op = newt_op_assign;
	}
	*ref = a;
}

static newt_poly_t
newt_call_builtin(const newt_builtin_t *builtin, newt_offset_t nactual)
{
	newt_poly_t ret = NEWT_ZERO;
	newt_poly_t *actuals = &newt_stack[newt_stackp - nactual];

	if (builtin->nformal < 0) {
		ret = (*builtin->funcv)(nactual, actuals);
	} else if (nactual != builtin->nformal) {
		newt_error("wrong number of args: wanted %d, got %d", builtin->nformal, nactual);
	} else {
		switch (builtin->nformal) {
		case 0:
			ret = builtin->func0();
			break;
		case 1:
			ret = builtin->func1(actuals[0]);
			break;
		case 2:
			ret = builtin->func2(actuals[0], actuals[1]);
			break;
		}
	}
	newt_stack_drop(nactual + 1);
	return ret;
}

newt_poly_t
newt_accumulator(void)
{
	return a;
}

void
newt_run_mark(void)
{
	newt_offset_t s;
	for (s = 0; s < newt_stackp; s++)
		newt_poly_mark(newt_stack[s], 1);
	newt_poly_mark(a, 1);
	newt_poly_mark(b, 1);
	if (code)
		newt_mark_addr(&newt_code_mem, code);
}

void
newt_run_move(void)
{
	newt_offset_t s;
	for (s = 0; s < newt_stackp; s++)
		newt_poly_move(&newt_stack[s], 1);
	newt_poly_move(&a, 1);
	newt_poly_move(&b, 1);
	if (code)
		newt_move_addr(&newt_code_mem, (void **) &code);
}

newt_poly_t
newt_code_run(newt_code_t *code_in)
{
	code = code_in;

	newt_poly_t	*ref = NULL;
	newt_id_t	id;
	newt_offset_t	ip = 0;
	newt_offset_t	o;

	while (code && !newt_abort) {
		while (ip < code->size && !newt_abort) {
#ifdef DEBUG_EXEC
			newt_code_dump_instruction(code, ip);
#endif
			newt_op_t op = code->code[ip++];
			bool push = (op & newt_op_push) != 0;
			op &= ~newt_op_push;
			switch(op) {
			case newt_op_num:
				memcpy(&a.f, &code->code[ip], sizeof(float));
				ip += sizeof(float);
				break;
			case newt_op_string:
				memcpy(&o, &code->code[ip], sizeof(newt_offset_t));
				ip += sizeof (newt_offset_t);
				a = newt_offset_to_poly(o, newt_string);
				break;
			case newt_op_list:
			case newt_op_tuple:
				memcpy(&o, &code->code[ip], sizeof(newt_offset_t));
				ip += sizeof (newt_offset_t);
				newt_list_t *l = newt_list_imm(o, op == newt_op_tuple);
				if (!l)
					newt_stack_drop(o);
				else
					a = newt_list_to_poly(l);
				break;
			case newt_op_id:
				memcpy(&id, &code->code[ip], sizeof(newt_id_t));
				ip += sizeof (newt_id_t);
				ref = newt_id_ref(id, false);
				if (ref) {
					a = *ref;
					break;
				}
				if (id < NEWT_BUILTIN_END) {
					a = newt_builtin_id_to_poly(id);
					break;
				}
				newt_error("undefined: %i", id);
				break;
			case newt_op_uminus:
			case newt_op_not:
				a = newt_unary(op, a);
				break;
			case newt_op_plus:
			case newt_op_minus:
			case newt_op_times:
			case newt_op_divide:
			case newt_op_div:
			case newt_op_mod:
			case newt_op_pow:
			case newt_op_eq:
			case newt_op_ne:
			case newt_op_is:
			case newt_op_is_not:
			case newt_op_in:
			case newt_op_not_in:
			case newt_op_lt:
			case newt_op_gt:
			case newt_op_ge:
			case newt_op_le:
			case newt_op_lxor:
			case newt_op_land:
			case newt_op_lor:
			case newt_op_array:
				b = newt_stack_pop();
				a = newt_binary(b, op, a, false);
				break;
			case newt_op_call:
				memcpy(&o, &code->code[ip], sizeof (newt_offset_t));
				ip += sizeof (newt_offset_t);
				a = newt_stack_pick(o);
				switch (newt_poly_type(a)) {
				case newt_func:
					if (!newt_func_push(newt_poly_to_func(a), o, code, ip - (1 + sizeof (newt_offset_t)))) {
						newt_stack_drop(o + 1);
						break;
					}
					a = newt_stack_pop();	/* get function back */
					code = newt_pool_ref(newt_poly_to_func(a)->code);
					ip = 0;
					push = false;	/* will pick up push on return */
					break;
				case newt_builtin:
					a = newt_call_builtin(newt_poly_to_builtin(a), o);
					break;
				default:
					newt_error("not a func: %p", a);
					newt_stack_drop(o + 1);
					break;
				}
				break;
			case newt_op_slice:
				a = newt_slice(a, code->code[ip]);
				ip++;
				break;
			case newt_op_assign:
			case newt_op_assign_plus:
			case newt_op_assign_minus:
			case newt_op_assign_times:
			case newt_op_assign_divide:
			case newt_op_assign_div:
			case newt_op_assign_mod:
			case newt_op_assign_pow:
			case newt_op_assign_land:
			case newt_op_assign_lor:
			case newt_op_assign_lxor:
			case newt_op_assign_lshift:
			case newt_op_assign_rshift:
				memcpy(&id, &code->code[ip], sizeof (newt_id_t));
				newt_assign(id, op);
				ip += sizeof (newt_id_t);
				break;
			case newt_op_global:
				memcpy(&id, &code->code[ip], sizeof (newt_id_t));
				newt_frame_mark_global(id);
				ip += sizeof (newt_id_t);
				break;
			case newt_op_branch_false:
				if (!newt_poly_true(a))
					memcpy(&ip, &code->code[ip], sizeof (newt_offset_t));
				else
					ip += sizeof (newt_offset_t);
				break;
			case newt_op_branch_true:
				if (newt_poly_true(a))
					memcpy(&ip, &code->code[ip], sizeof (newt_offset_t));
				else
					ip += sizeof (newt_offset_t);
				break;
			case newt_op_branch:
				memcpy(&ip, &code->code[ip], sizeof (newt_offset_t));
				break;
			case newt_op_range_start:
				memcpy(&id, &code->code[ip], sizeof (newt_id_t));
				ip += sizeof (newt_id_t);
				memcpy(&o, &code->code[ip], sizeof (newt_offset_t));
				ip += sizeof (newt_offset_t);
				newt_range_start(id, o);
				newt_stack_drop(o);
				break;
			case newt_op_range_step:
				if (!newt_range_step())
					memcpy(&ip, &code->code[ip], sizeof (newt_offset_t));
				else
					ip += sizeof (newt_offset_t);
				break;
			case newt_op_in_start:
				memcpy(&id, &code->code[ip], sizeof (newt_id_t));
				ip += sizeof (newt_id_t);
				newt_in_start(id);
				newt_stack_pop();
				break;
			case newt_op_in_step:
				if (!newt_in_step())
					memcpy(&ip, &code->code[ip], sizeof (newt_offset_t));
				else
					ip += sizeof (newt_offset_t);
				break;
			case newt_op_line:
				memcpy(&o, &code->code[ip], sizeof (newt_offset_t));
				newt_line = o;
				ip += sizeof (newt_offset_t);
				break;
			default:
				break;
			}
			if (push)
				newt_stack_push(a);
#ifdef DEBUG_EXEC
			printf("\t\ta= "); newt_poly_print(stdout, a);
			for (int i = newt_stackp; i;) {
				printf(", [%d]= ", newt_stackp - i);
				newt_poly_print(stdout, newt_stack[--i]);
			}
			printf("\n");
#endif
		}
		code = newt_frame_pop(&ip);
		if (code) {
			newt_op_t op = code->code[ip++];
			bool push = (op & newt_op_push) != 0;
			op &= ~newt_op_push;
			switch (op) {
			case newt_op_call:
				ip += sizeof (newt_offset_t);
				break;
			default:
				break;
			}
			if (push)
				newt_stack_push(a);
		}
	}
	return a;
}

static newt_offset_t
newt_code_size(void *addr)
{
	newt_code_t *code = addr;

	return (newt_offset_t) sizeof (newt_code_t) + code->size;
}

static void
code_mark(uint8_t *code, newt_offset_t size)
{
	newt_offset_t ip = 0;

	while (ip < size) {
		newt_op_t op = code[ip++] & ~newt_op_push;
		newt_offset_t o;
		switch (op) {
		case newt_op_string:
			memcpy(&o, &code[ip], sizeof (newt_offset_t));
			newt_mark_offset(&newt_string_mem, o);
			break;
		default:
			break;
		}
		ip += newt_op_extra_size(op);
	}
}

static void
code_move(uint8_t *code, newt_offset_t size)
{
	newt_offset_t	ip = 0;

	while (ip < size) {
		newt_op_t op = code[ip++] & ~newt_op_push;
		newt_offset_t o, p;
		switch (op) {
		case newt_op_string:
			memcpy(&o, &code[ip], sizeof (newt_offset_t));
			p = o;
			newt_move_block_offset(&p);
			if (o != p)
				memcpy(&code[ip], &p, sizeof (newt_offset_t));
			break;
		default:
			break;
		}
		ip += newt_op_extra_size(op);
	}
}

static void
newt_code_mark(void *addr)
{
	newt_code_t	*code = addr;

	code_mark(code->code, code->size);
}

static void
newt_code_move(void *addr)
{
	newt_code_t	*code = addr;

	code_move(code->code, code->size);
}

const newt_mem_t newt_code_mem = {
	.size = newt_code_size,
	.mark = newt_code_mark,
	.move = newt_code_move,
	NEWT_MEM_DECLARE_NAME("code")
};

static newt_offset_t
newt_compile_size(void *addr)
{
	(void) addr;
	return compile_alloc;
}

static void
newt_compile_mark(void *addr)
{
	code_mark(addr, compile_size);
}

static void
newt_compile_move(void *addr)
{
	code_move(addr, compile_size);
}

const newt_mem_t newt_compile_mem = {
	.size = newt_compile_size,
	.mark = newt_compile_mark,
	.move = newt_compile_move,
	NEWT_MEM_DECLARE_NAME("compile")
};
