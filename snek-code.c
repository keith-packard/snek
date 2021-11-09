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

#ifdef DEBUG_COMPILE
static void
snek_code_dump(snek_code_t *code);
#endif

/*
 * Temporary bytecode storage for the compiler
 */

uint8_t			*snek_compile;			/* bytecode  */
static snek_offset_t	compile_alloc;			/* space allocated for snek_compile */
snek_offset_t		snek_compile_size;		/* space used in snek_compile */
snek_offset_t		snek_compile_prev;		/* offset of previous instruction */
snek_offset_t		snek_compile_prev_prev;		/* offset of instruction before previous instruction */

#define COMPILE_INC	32				/* number of bytes added to snek_compile when it fills */

/*
 * Compute the size of all operands for an opcode
 */
static uint8_t
snek_op_operand_size(snek_op_t op)
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
	case snek_op_del:
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
	case snek_op_chain_eq:
	case snek_op_chain_ne:
	case snek_op_chain_gt:
	case snek_op_chain_lt:
	case snek_op_chain_ge:
	case snek_op_chain_le:
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

/*
 * Extend the temporary space used for bytecode to make
 * space for 'n' more bytes. If 'data' is provided, copy that
 * into the new space
 */

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
	memcpy(snek_compile + snek_compile_size, data, n);
	snek_compile_size += n;
}

/*
 * Remove the previous instruction by resetting the snek_compile_size
 * and snek_compile_prev values. Note that snek_compile_prev_prev is
 * not valid after this call, so you can't do this more than once or
 * snek_compile_prev will become invalid too.
 */
void
snek_code_delete_prev(void)
{
	snek_compile_size = snek_compile_prev;
	snek_compile_prev = snek_compile_prev_prev;
}

/*
 * Add an opcode to the current bytecode
 */
void
snek_code_add_op(snek_op_t op)
{
	snek_compile_prev_prev = snek_compile_prev;
	snek_compile_prev = snek_compile_size;
	compile_extend(1, &op);
}

/*
 * Add an instruction with an 'offset' parameter to the current
 * bytecode
 */
void
snek_code_add_op_offset(snek_op_t op, snek_offset_t o)
{
	snek_code_add_op(op);
	compile_extend(sizeof (snek_offset_t), &o);
}

/*
 * Add an instruction with a 'uint8_t' parameter to the current
 * bytecode
 */
void
snek_code_add_op_uint8(snek_op_t op, uint8_t u8)
{
	snek_code_add_op(op);
	compile_extend(sizeof(uint8_t), &u8);
}

/*
 * Add an immediate number to the current bytecode.
 *
 * If that number can be represented exactly by a signed
 * 8-bit value, use that instead of a full 32-bit float
 */
void
snek_code_add_number(float number)
{
	int8_t i8 = (int8_t) number;
	if ((float) i8 == number) {
		snek_code_add_op_uint8(snek_op_int, (uint8_t) i8);
	} else {
		snek_code_add_op(snek_op_num);
		compile_extend(sizeof(float), &number);
	}
}

/*
 * Add an immediate string to the current bytecode
 */
void
snek_code_add_string(char *string)
{
	snek_offset_t s;

	snek_stack_push_string(string);
	snek_code_add_op_offset(snek_op_string, 0);
	s = snek_pool_offset(snek_stack_pop_string(string));
	memcpy(snek_compile + snek_compile_prev + 1, &s, sizeof (snek_offset_t));
}

/*
 * Add code for the start of a 'for i in range' loop
 */
void
snek_code_add_in_range(snek_id_t id, uint8_t nactual, uint8_t for_depth)
{
	/* Initialize the range operation */
	snek_code_add_op_offset(snek_op_range_start, nactual);
	compile_extend(sizeof (uint8_t), &for_depth);
	compile_extend(sizeof (snek_id_t), &id);

	/* Build the range step */
	snek_code_add_op_offset(snek_op_range_step, 0);
	compile_extend(sizeof (uint8_t), &for_depth);
	compile_extend(sizeof (snek_id_t), &id);
}

/*
 * Add code for the start of a 'for i in expr' loop
 */
void
snek_code_add_in_enum(snek_id_t id, uint8_t for_depth)
{
	snek_id_t	list;
	snek_id_t	i;

	/* list holds the result of the expr */
	list = snek_for_tmp(for_depth, 0);
	/* i holds the index into list */
	i = snek_for_tmp(for_depth, 1);

	/* Initialize list with the expr parameter */
	snek_code_add_op_id(snek_op_assign, list);

	/* Initialize i to start at 0 */
	snek_code_add_number(0.0f);
	snek_code_add_op_id(snek_op_assign, i);

	/* Here's the top of the loop, which fetches the next element
	 * of the list, terminating when done
	 */
	snek_code_add_op_offset(snek_op_in_step, 0);
	compile_extend(sizeof(uint8_t), &for_depth);
	compile_extend(sizeof(snek_id_t), &id);
}

void
snek_code_add_line(void)
{
	snek_code_add_op_offset(snek_op_line, snek_lex_line);
}

/*
 * Patch pending branch once the target instruction is known
 */
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
			if ((snek_forward_t) (f & 0xff) == forward) {
				snek_compile[ip-1] = f >> 8 | push;
				memcpy(&snek_compile[ip], &target, sizeof(snek_offset_t));
			}
			break;
		default:
			break;
		}
		ip += snek_op_operand_size(op);
	}
}

/*
 * Reset compiler state to mark the compiler buffer as empty
 */
void
snek_code_reset(void)
{
	snek_compile_size = 0;
	compile_alloc = 0;
	snek_compile = NULL;
}

/*
 * Construct a code object from the current bytecode buffer
 */
snek_code_t *
snek_code_finish(void)
{
	if (snek_compile_size == 0)
		return NULL;
	snek_code_t *code = snek_alloc(sizeof (snek_code_t) + snek_compile_size);

	if (code) {
		memcpy(&code->code, snek_compile, snek_compile_size);
		code->size = snek_compile_size;
#ifdef DEBUG_COMPILE
		snek_code_dump(code);
#endif
	}
	snek_code_reset();
	return code;
}

/*
 * Find the first line in the specified code block. This is
 * used when printing out function objects
 */
snek_offset_t
snek_code_line(snek_code_t *code)
{
	snek_offset_t	ip;
	snek_offset_t	line = 0;
	snek_op_t	op;

	for (ip = 0; ip < code->size; ip += snek_op_operand_size(op)) {
		op = code->code[ip++];
		if (op == snek_op_line) {
			memcpy(&line, &code->code[ip], sizeof (snek_offset_t));
			break;
		}
	}
	return line;
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
			snek_mark_offset(snek_mems(snek_string), o);
			break;
		default:
			break;
		}
		ip += snek_op_operand_size(op);
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
		ip += snek_op_operand_size(op);
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

#if defined(DEBUG_COMPILE) || defined(DEBUG_EXEC)

#define dbg(fmt, args...) fprintf(stderr, fmt, ## args)

static const char * const snek_op_names[] = {
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

	[snek_op_chain_eq] = "chain eq",
	[snek_op_chain_ne] = "chain ne",
	[snek_op_chain_gt] = "chain gt",
	[snek_op_chain_lt] = "chain lt",
	[snek_op_chain_ge] = "chain ge",
	[snek_op_chain_le] = "chain le",

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
	[snek_op_del] = "del",
	[snek_op_assert] = "assert",
	[snek_op_branch] = "branch",
	[snek_op_branch_true] = "branch_true",
	[snek_op_branch_false] = "branch_false",
	[snek_op_forward] = "forward",
	[snek_op_range_start] = "range_start",
	[snek_op_range_step] = "range_step",
	[snek_op_in_step] = "in_step",
	[snek_op_return] = "return",

	[snek_op_null] = "null",
	[snek_op_nop] = "nop",
	[snek_op_line] = "line",
};

snek_offset_t
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
		dbg("%s\n", (char *) snek_pool_addr(o));
		break;
	case snek_op_list:
	case snek_op_tuple:
		memcpy(&o, &code->code[ip], sizeof(snek_offset_t));
		dbg("%u\n", o);
		break;
	case snek_op_id:
	case snek_op_del:
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
		dbg("%d position %d named\n", o & 0xff, o >> 8);
		break;
	case snek_op_slice:
		if (code->code[ip] & SNEK_OP_SLICE_START) dbg(" start");
		if (code->code[ip] & SNEK_OP_SLICE_END) dbg(" end");
		if (code->code[ip] & SNEK_OP_SLICE_STRIDE) dbg(" stride");
		break;
	case snek_op_chain_eq:
	case snek_op_chain_ne:
	case snek_op_chain_gt:
	case snek_op_chain_lt:
	case snek_op_chain_ge:
	case snek_op_chain_le:
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
	return ip + snek_op_operand_size(op);
}

#endif

#ifdef DEBUG_COMPILE

static void
snek_code_dump(snek_code_t *code)
{
	snek_offset_t	ip = 0;

	while (ip < code->size) {
		ip = snek_code_dump_instruction(code, ip);
	}
}

#endif
