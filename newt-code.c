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
	memcpy(compile + compile_size, data, n);
	compile_size += n;
	return start;
}

newt_offset_t
newt_code_add_op(newt_op_t op)
{
	return compile_extend(1, &op);
}

newt_offset_t
newt_code_add_op_name(newt_op_t op, int atom)
{
	return compile_extend(1, &op);
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

static newt_poly_t
newt_binary(newt_poly_t a, newt_op_t op, newt_poly_t b)
{
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
		default:
			break;
		}
		a = newt_float_to_poly(af);
	}
	return a;
}

static newt_poly_t
newt_uminus(newt_poly_t a)
{
	if (newt_is_float(a)) {
		float af = newt_poly_to_float(a);
		af = -af;
		a = newt_float_to_poly(af);
	}
	return a;
}

#define NEWT_STACK	32
static newt_poly_t	newt_stack[NEWT_STACK];
static newt_offset_t	newt_stackp = 0;

newt_poly_t
newt_code_run(newt_code_t *code)
{
	newt_poly_t 	a = NEWT_ZERO, b = NEWT_ZERO;
	newt_offset_t	ip = 0;

	while (ip < code->size) {
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
		case newt_op_uminus:
			a = newt_uminus(a);
			break;
		case newt_op_plus:
		case newt_op_minus:
		case newt_op_times:
		case newt_op_divide:
		case newt_op_mod:
			b = newt_stack[--newt_stackp];
			a = newt_binary(a, op, b);
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
	newt_code_t *code = addr;
	(void) code;
}

static void
newt_code_move(void *addr)
{
	newt_code_t *code = addr;
	(void) code;
}

const newt_mem_t newt_code_mem = {
	.size = newt_code_size,
	.mark = newt_code_mark,
	.move = newt_code_move,
	.name = "code",
};
