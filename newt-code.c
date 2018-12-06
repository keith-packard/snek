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

static uint8_t		*compile;
static newt_offset_t	compile_size, compile_alloc;

#define COMPILE_INC	32

static newt_offset_t
compile_extend(newt_offset_t n, void *data)
{
	newt_offset_t start = compile_size;

	if (compile_size + n > compile_alloc) {
		uint8_t *new_compile = newt_alloc(compile_alloc + COMPILE_INC);
		if (!new_compile)
			return 0xffff;
		memcpy(new_compile, compile, compile_size);
		compile_alloc += COMPILE_INC;
		compile = new_compile;
	}
	if (data)
		memcpy(compile + compile_size, data, n);
	compile_size += n;
	return start;
}

newt_offset_t
newt_code_current(void)
{
	return compile_size;
}

newt_offset_t
newt_code_add_op(newt_op_t op)
{
	return compile_extend(1, &op);
}

newt_offset_t
newt_code_add_op_id(newt_op_t op, newt_id_t id)
{
	newt_offset_t offset = compile_extend(1, &op);
	compile_extend(sizeof (newt_id_t), &id);
	return offset;
}

newt_offset_t
newt_code_add_number(float number)
{
	newt_op_t op = newt_op_num;
	newt_offset_t offset;

	offset = compile_extend(1, &op);
	compile_extend(sizeof(float), &number);
	return offset;
}

newt_offset_t
newt_code_add_op_branch(newt_op_t op)
{
	newt_offset_t offset;

	offset = compile_extend(1, &op);
	compile_extend(sizeof (newt_offset_t), NULL);
	return offset;
}

void
newt_code_patch_branch(newt_offset_t branch, newt_offset_t target)
{
	memcpy(compile + branch + 1, &target, sizeof (newt_offset_t));
}

void
newt_code_set_push(newt_offset_t offset)
{
	compile[offset] |= newt_op_push;
}

newt_code_t *
newt_code_finish(void)
{
	newt_code_t *code = newt_alloc(sizeof (newt_code_t) + compile_size);
	memcpy(&code->code, compile, compile_size);
	code->size = compile_size;
	compile_size = 0;
	return code;
}

static bool
newt_true(newt_poly_t a)
{
	if (newt_is_float(a))
		return newt_poly_to_float(a) != 0.0f;
	return !newt_is_null(a);
}

static float
newt_bool_float(bool b)
{
	return b ? 1.0f : 0.0f;
}

static newt_poly_t
newt_bool(bool b)
{
	return newt_float_to_poly(b ? 1.0f : 0.0f);
}

#define DEBUG_COMPILE
#if defined(DEBUG_COMPILE) || defined(DEBUG_EXEC)

static const char *newt_op_names[] = {
	[newt_op_nop] = "nop",
	[newt_op_num] = "num",
	[newt_op_id] = "id",

	[newt_op_and] = "and",
	[newt_op_or] = "or",
	[newt_op_not] = "not",

	[newt_op_eq] = "eq",
	[newt_op_ne] = "ne",
	[newt_op_gt] = "gt",
	[newt_op_lt] = "lt",
	[newt_op_ge] = "ge",
	[newt_op_le] = "le",

	[newt_op_plus] = "plus",
	[newt_op_minus] = "minus",
	[newt_op_times] = "times",
	[newt_op_divide] = "divide",
	[newt_op_mod] = "mod",

	[newt_op_uminus] = "uminus",

	[newt_op_assign] = "assign",

	[newt_op_if] = "if",
	[newt_op_branch] = "branch",
};

newt_offset_t
newt_code_dump_instruction(newt_code_t *code, newt_offset_t ip)
{
	newt_poly_t	a;
	newt_id_t	id;
	newt_offset_t	o;

	printf("%6d:  ", ip);
	newt_op_t op = code->code[ip++];
	bool push = (op & newt_op_push) != 0;
	op &= ~newt_op_push;
	printf("%-12s %c ", newt_op_names[op], push ? '^' : ' ');
	switch(op) {
	case newt_op_num:
		memcpy(&a, &code->code[ip], sizeof(float));
		ip += sizeof(float);
		printf("%g\n", a.f);
		break;
	case newt_op_id:
	case newt_op_assign:
		memcpy(&id, &code->code[ip], sizeof(newt_id_t));
		ip += sizeof (newt_id_t);
		printf("%s\n", newt_name_string(id));
		break;
	case newt_op_if:
	case newt_op_branch:
		memcpy(&o, &code->code[ip], sizeof (newt_offset_t));
		printf("%d\n", o);
		ip += sizeof (newt_offset_t);
		break;
	default:
		printf("\n");
		break;
	}
	return ip;
}
#endif


#ifdef DEBUG_COMPILE
void
newt_code_dump(newt_code_t *code)
{
	newt_offset_t	ip = 0;

	while (ip < code->size) {
		ip = newt_code_dump_instruction(code, ip);
	}
}

#else
#define newt_code_dump(code)
#endif

static newt_poly_t
newt_binary(newt_poly_t a, newt_op_t op, newt_poly_t b)
{
	switch (op) {
	case newt_op_and:
		return newt_bool(newt_true(a) && newt_true(b));
	case newt_op_or:
		return newt_bool(newt_true(a) || newt_true(b));
	default:
		break;
	}
	if (newt_is_float(a) && newt_is_float(b)) {
		float	af = newt_poly_to_float(a);
		float	bf = newt_poly_to_float(b);
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
		case newt_op_mod:
			af = (float) ((int32_t) af % (int32_t) bf);
			break;
		case newt_op_eq:
			af = newt_bool_float(af == bf);
			break;
		case newt_op_ne:
			af = newt_bool_float(af != bf);
			break;
		case newt_op_lt:
			af = newt_bool_float(af < bf);
			break;
		case newt_op_gt:
			af = newt_bool_float(af > bf);
			break;
		case newt_op_le:
			af = newt_bool_float(af <= bf);
			break;
		case newt_op_ge:
			af = newt_bool_float(af >= bf);
			break;
		default:
			break;
		}
		a = newt_float_to_poly(af);
	}
	return a;
}

static newt_poly_t
newt_unary(newt_op_t op, newt_poly_t a)
{
	switch (op) {
	case newt_op_not:
		return newt_bool(!newt_true(a));
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

static newt_poly_t
newt_fetch(newt_id_t id)
{
	newt_variable_t *v = newt_frame_lookup(id, false);
	if (!v)
		return NEWT_ZERO;
	return v->value;
}

static void
newt_assign(newt_id_t id, newt_poly_t a)
{
	newt_variable_t *v = newt_frame_lookup(id, true);
	if (v)
		v->value = a;
}

#define NEWT_STACK	32
static newt_poly_t	newt_stack[NEWT_STACK];
static newt_offset_t	newt_stackp = 0;

int
newt_stack_size(void *addr)
{
	(void) addr;
	return 0;
}

void
newt_stack_mark(void *addr)
{
	newt_offset_t s;
	(void) addr;
	for (s = 0; s < newt_stackp; s++)
		newt_poly_mark(newt_stack[s], 1);
}

void
newt_stack_move(void *addr)
{
	newt_offset_t s;
	(void) addr;
	for (s = 0; s < newt_stackp; s++)
		newt_poly_move(&newt_stack[s], 1);
}

const newt_mem_t newt_stack_mem = {
	.size = newt_stack_size,
	.mark = newt_stack_mark,
	.move = newt_stack_move,
	.name = "stack",
};

newt_poly_t
newt_code_run(newt_code_t *code)
{
	newt_poly_t 	a = NEWT_ZERO, b = NEWT_ZERO;
	newt_id_t	id;
	newt_offset_t	ip = 0;

	newt_code_dump(code);
	while (ip < code->size) {
#if DEBUG_EXEC
		newt_code_dump_instruction(code, ip);
		printf("\t\t"); newt_poly_print(a); printf("\n");
#endif
		newt_op_t op = code->code[ip++];
		bool push = (op & newt_op_push) != 0;
		op &= ~newt_op_push;
		switch(op) {
		case newt_op_nop:
			break;
		case newt_op_num:
			memcpy(&a, &code->code[ip], sizeof(float));
			ip += sizeof(float);
			break;
		case newt_op_id:
			memcpy(&id, &code->code[ip], sizeof(newt_id_t));
			ip += sizeof (newt_id_t);
			a = newt_fetch(id);
			break;
		case newt_op_uminus:
		case newt_op_not:
			a = newt_unary(op, a);
			break;
		case newt_op_plus:
		case newt_op_minus:
		case newt_op_times:
		case newt_op_divide:
		case newt_op_mod:
		case newt_op_eq:
		case newt_op_ne:
		case newt_op_lt:
		case newt_op_gt:
		case newt_op_ge:
		case newt_op_le:
		case newt_op_and:
		case newt_op_or:
			b = newt_stack[--newt_stackp];
			a = newt_binary(b, op, a);
			break;
		case newt_op_assign:
			memcpy(&id, &code->code[ip], sizeof (newt_id_t));
			newt_assign(id, a);
			ip += sizeof (newt_id_t);
			break;
		case newt_op_if:
			if (!newt_true(a))
				memcpy(&ip, &code->code[ip], sizeof (newt_offset_t));
			else
				ip += sizeof (newt_offset_t);
			break;
		case newt_op_branch:
			memcpy(&ip, &code->code[ip], sizeof (newt_offset_t));
			break;
		default:
			break;
		}
		if (push)
			newt_stack[newt_stackp++] = a;
	}
	return a;
}

static int
newt_code_size(void *addr)
{
	newt_code_t *code = addr;

	return sizeof (newt_code_t) + code->size;
}

static void
newt_code_mark(void *addr)
{
	(void) addr;
}

static void
newt_code_move(void *addr)
{
	(void) addr;
}

const newt_mem_t newt_code_mem = {
	.size = newt_code_size,
	.mark = newt_code_mark,
	.move = newt_code_move,
	.name = "code",
};
