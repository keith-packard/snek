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

/*
 * Execution state
 */
snek_offset_t 	snek_line;		/* current line number */
snek_poly_t	snek_stack[SNEK_STACK];	/* value stack */
snek_offset_t	snek_stackp;		/* value stack pointer  */
snek_poly_t 	snek_a = SNEK_NULL;	/* accumulator */
snek_code_t	*snek_code;		/* current code pointer */

/*
 * Push a value to the stack, raise an error if the stack overflows
 */
void
snek_stack_push(snek_poly_t p)
{
	if (snek_stackp == SNEK_STACK) {
		snek_error_0("stack overflow");
		return;
	}
	snek_stack[snek_stackp++] = p;
}

/*
 * Pop a value from the stack
 */
snek_poly_t
snek_stack_pop(void)
{
#if SNEK_DEBUG
	if (!snek_stackp)
		snek_panic("stack underflow");
#endif
	return snek_stack[--snek_stackp];
}

/*
 * Fetch a value from the middle of the stack.
 * 'off' is the number of stack elements above the current
 * top of stack
 */
snek_poly_t
snek_stack_pick(snek_offset_t off)
{
#if SNEK_DEBUG
	if (off >= snek_stackp)
		snek_panic("stack underflow");
#endif
	return snek_stack[snek_stackp - off - 1];
}

/*
 * Discard a number of elements from the stack
 */
void
snek_stack_drop(snek_offset_t off)
{
#if SNEK_DEBUG
	if (off > snek_stackp)
		snek_panic("stack underflow");
#endif
	snek_stackp -= off;
}

/*
 * Pop a float from the stack. Raise an error if the
 * value on  the top isn't a number
 */
float
snek_stack_pop_float(void)
{
	return snek_poly_get_float(snek_stack_pop());
}

/*
 * Pop an soffset from the stack. Raise an error if the
 * value on  the top isn't a number
 */
snek_soffset_t
snek_stack_pop_soffset(void)
{
	return (snek_soffset_t) snek_stack_pop_float();
}

/*
 * Start a 'for i in range' statement
 */
static void
snek_range_start(snek_offset_t ip)
{
	snek_offset_t	nactual;	/* number of actuals passed to 'range' */
	snek_id_t	id;		/* variable used in the 'for' statement */
	uint8_t		for_depth;	/* loop nesting depth (used to build temp var names) */

	/* Fetch params from instruction */
	memcpy(&nactual, &snek_code->code[ip], sizeof(snek_offset_t));
	memcpy(&id, &snek_code->code[ip + sizeof(snek_offset_t) + sizeof (uint8_t)], sizeof (snek_id_t));
	memcpy(&for_depth, &snek_code->code[ip + sizeof(snek_offset_t)], sizeof (uint8_t));

	/* Compute the loop parameters given the actuals provided to the range function */
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
		current = snek_stack_pop_float();
		if (step == 0) {
			snek_error_step();
			return;
		}
		break;
	default:
		snek_stack_drop(nactual);
		snek_error_args(3, nactual);
		return;
	}

	/* Assign initial value (current - step) */

	snek_poly_t	*id_ref = snek_id_ref(id, true);
	if(!id_ref)
		return;
	*id_ref = snek_float_to_poly(current - step);

	/* Save limit in tmp variable */
	snek_poly_t	*limit_ref = snek_id_ref(snek_for_tmp(for_depth, 0), true);
	if (!limit_ref)
		return;
	*limit_ref = snek_float_to_poly(limit);

	/* Save step in tmp variable */
	snek_poly_t	*step_ref = snek_id_ref(snek_for_tmp(for_depth, 1), true);
	if (!step_ref)
		return;
	*step_ref = snek_float_to_poly(step);

	return;
}

/*
 * Next step in a 'for i in range' statement
 */
static bool
snek_range_step(snek_offset_t ip)
{
	uint8_t		for_depth;	/* nesting depth of loop */
	snek_id_t	id;		/* id of the 'for' variable */

	memcpy(&for_depth, &snek_code->code[ip + sizeof(snek_offset_t)], sizeof (uint8_t));
	memcpy(&id, &snek_code->code[ip + sizeof(snek_offset_t) + sizeof (uint8_t)], sizeof (snek_id_t));

	/* Go get refernences to all three variables */
	snek_poly_t	*id_ref = snek_id_ref(id, false);
	snek_poly_t	*limit_ref = snek_id_ref(snek_for_tmp(for_depth, 0), false);
	snek_poly_t	*step_ref = snek_id_ref(snek_for_tmp(for_depth, 1), false);

	if (!id_ref || !limit_ref || !step_ref)
		return false;

	/* Compute the next value in the sequence */
	float step = snek_poly_get_float(*step_ref);
	float value = snek_poly_get_float(*id_ref) + step;
	*id_ref = snek_float_to_poly(value);

	/* Check to see if we're done */
	float limit = snek_poly_get_float(*limit_ref);
	if (step > 0 ? value >= limit : value <= limit)
		return false;

	/* keep going */
	return true;
}

/*
 * Next step in a 'for i in expr' statement
 */
static bool
snek_in_step(snek_offset_t ip)
{
	uint8_t		for_depth;	/* nesting depth of loop */
	snek_id_t	id;		/* id of the 'for' variable */
	snek_poly_t	*ref;		/* reference to the 'for' variable storage */

	memcpy(&for_depth, &snek_code->code[ip + sizeof(snek_offset_t)], sizeof(uint8_t));

	/* Get current index, save next index */
	snek_poly_t *i_ref = snek_id_ref(snek_for_tmp(for_depth, 1), false);
	snek_soffset_t i = snek_poly_get_soffset(*i_ref);
	*i_ref = snek_soffset_to_poly(i + 1);

	/* Fetch iterable */
	snek_poly_t array = *snek_id_ref(snek_for_tmp(for_depth, 0), false);

	/* Compute current value */
	snek_poly_t value = SNEK_NULL;

	snek_list_t *list;

	/* Get the current value out of the object */
	switch (snek_poly_type(array)) {
	case snek_list:
		list = snek_poly_to_list(array);
#ifndef SNEK_NO_DICT
		if (snek_list_type(list) == snek_list_dict)
			i *= 2;
#endif
		if ((snek_offset_t) i < list->size)
			value = snek_list_data(list)[(snek_offset_t) i];
		break;
	case snek_string:
		value = snek_string_get(snek_poly_to_string(array), snek_soffset_to_poly(i), false);
		break;
	default:
		snek_error_type_1(array);
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

/*
 * Cast a float to a 32-bit int, raising an exception if
 * the value cannot be represented exactly
 */
static int32_t __attribute__((noinline))
snek_float_to_int(float f)
{
	int32_t i = (int32_t) f;
	if (i != f)
		snek_error_value(snek_float_to_poly(f));
	return i;
}

/*
 * Perform a binary operation.
 *
 * The 'inplace' parameter indicates whether this
 * is an 'enhanced assignment' operator or a regular
 * binary operator. This makes a difference for
 * mutable values
 */
static snek_poly_t
snek_binary(snek_poly_t a, snek_op_t op, snek_poly_t b, bool inplace)
{
	snek_list_t	*al;
	snek_list_t	*bl;
	float		af;
	float		bf;
	bool		found;
	snek_poly_t	ret = SNEK_INVALID;

	/* Compare operators work between any two values
	 * in snek, so no type checking needed here.
	 */
	if (op <= snek_op_is_not) {
		bool v;
		if (op < snek_op_is && (snek_is_nan(a) || snek_is_nan(b)))
			v = (op == snek_op_ne);
		else
		{
			int8_t cmp = snek_poly_cmp(a, b, op >= snek_op_is);
			switch (op) {
			case snek_op_eq:
			case snek_op_is:
				v = cmp == 0;
				break;
			case snek_op_ne:
			case snek_op_is_not:
				v = cmp != 0;
				break;
			case snek_op_gt:
				v = cmp > 0;
				break;
			case snek_op_lt:
				v = cmp < 0;
				break;
			case snek_op_ge:
				v = cmp >= 0;
				break;
			case snek_op_le:
			default:
				v = cmp <= 0;
				break;
			}
		}
		return snek_bool_to_poly(v);
	}

	/* Now a bunch of random logic to
	 * figure out how to combine the two types and operator
	 */

	snek_type_t	at = snek_poly_type(a);
	snek_type_t	bt = snek_poly_type(b);

	if (op == snek_op_array) {
		switch (at) {
		case snek_list:
			ret = snek_list_get(snek_poly_to_list(a), b, true);
			break;
		case snek_string:
			ret = snek_string_get(snek_poly_to_string(a), b, true);
			break;
		default:
			break;
		}
	} else if (at == snek_float && bt == snek_float) {

		/* two numbers and an operator */

		af = snek_poly_to_float(a);
		bf = snek_poly_to_float(b);
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
			af = floorf(af / bf);
			break;
		case snek_op_mod:
			af = af - floorf(af/bf) * bf;
			break;
		case snek_op_pow:
			af = powf(af, bf);
			break;
		case snek_op_land:
			af = (float) (snek_float_to_int(af) & snek_float_to_int(bf));
			break;
		case snek_op_lor:
			af = (float) (snek_float_to_int(af) | snek_float_to_int(bf));
			break;
		case snek_op_lxor:
			af = (float) (snek_float_to_int(af) ^ snek_float_to_int(bf));
			break;
		case snek_op_lshift:
			af = (float) (snek_float_to_int(af) << snek_float_to_int(bf));
			break;
		case snek_op_rshift:
			af = (float) (snek_float_to_int(af) >> snek_float_to_int(bf));
			break;
		default:
			break;
		}
		ret = snek_float_to_poly(af);
	} else {

		/* The rest of the the operators; each operator has
		 * it's own typechecking
		 */

		switch (op) {
		case snek_op_in:
		case snek_op_not_in:
			switch (bt) {
			case snek_list:
				bl = snek_poly_to_list(b);
				snek_offset_t o, step =
#ifndef SNEK_NO_DICT
					snek_list_type(bl) == snek_list_dict ? 2 :
#endif
					1;
				found = false;
				for (o = 0; o < bl->size; o += step) {
					if (snek_poly_cmp(a, snek_list_data(bl)[o], false) == 0) {
						found = true;
						break;
					}
				}
				ret = snek_bool_to_poly(found == (op == snek_op_in));
				break;
			case snek_string:
				if (at == snek_string) {
					found = strstr(snek_poly_to_string(b), snek_poly_to_string(a)) != NULL;
					ret = snek_bool_to_poly(found == (op == snek_op_in));
				}
				break;
			default:
				break;
			}
			break;
		case snek_op_plus:
			if (at != bt)
				break;
			switch (at) {
			case snek_string:
				ret = snek_string_cat(snek_poly_to_string(a),
						      snek_poly_to_string(b));
				break;
			case snek_list:
				al = snek_poly_to_list(a);
				bl = snek_poly_to_list(b);

				if (snek_list_type(al) == snek_list_type(bl)
#ifndef SNEK_NO_DICT
				    && snek_list_type(al) != snek_list_dict
#endif
					)
				{
					if (inplace && !snek_list_readonly(al))
						al = snek_list_append(al, bl);
					else
						al = snek_list_plus(al, bl);
					ret = snek_list_to_poly(al);
				}
				break;
			default:
				break;
			}
			break;
		case snek_op_times:
			if (bt == snek_float) {
				snek_soffset_t bo = snek_poly_get_soffset(b);
				if (bo < 0)
					ret = SNEK_NULL;
				else {
					switch (at) {
					case snek_list:
						al = snek_poly_to_list(a);
#ifndef SNEK_NO_DICT
						if (snek_list_type(al) != snek_list_dict)
#endif
							ret = snek_list_to_poly(snek_list_times(al, bo));
						break;
					case snek_string:
						ret = snek_string_times(snek_poly_to_string(a), bo);
						break;
					default:
						break;
					}
				}
			}
			break;
		case snek_op_mod:
			if (at == snek_string)
				ret = snek_string_interpolate(snek_poly_to_string(a), b);
			break;
		default:
			break;
		}
	}

	/* If we haven't computed any return, raise an exception */
	if (snek_is_invalid(ret))
		return snek_error_type_2(a, b);
	return ret;
}

static inline snek_soffset_t
soffset_sgn(snek_soffset_t s) {
	return (s > 0) - (s < 0);
}

#ifndef SNEK_NO_SLICE
/*
 * Slice operator
 */
static void
snek_slice(uint8_t bits)
{
	/* Start and end are set to SNEK_OFFSET_NONE by default to let
	 * the code select suitable defaults based on the stride
	 */
	snek_soffset_t	start = SNEK_SOFFSET_NONE;	/* Start of the slice */
	snek_soffset_t	end = SNEK_SOFFSET_NONE;	/* End of the slice */
	snek_soffset_t	stride = 1;			/* Stride within the slice */
	snek_soffset_t	len;				/* Length of the composite object */

	/* Pull out the values which were in the slice specification */

	if (bits & SNEK_OP_SLICE_STRIDE)
		stride = snek_stack_pop_soffset();

	if (bits & SNEK_OP_SLICE_END)
		end = snek_stack_pop_soffset();

	if (bits & SNEK_OP_SLICE_START)
		start = snek_stack_pop_soffset();

	/* Get the composite value which is getting sliced */
	snek_a = snek_stack_pop();

	len = snek_poly_len(snek_a);

	/* A stride of zero is illegal */
	if (stride == 0) {
		snek_error_step();
		return;
	}

	/* Negative positions are relative to end */
	if (start < 0)
		start = len + start;

	if (end < 0)
		end = len + end;

	/* Select start and end values based on stride
	 * if they weren't specified in the operation
	 */
	if (stride > 0) {
		/* Use zero by default, or if negative */
		if (start == SNEK_SOFFSET_NONE || start < 0)
			start = 0;

		/* use len by default or if too big */
		if (end == SNEK_SOFFSET_NONE || end > len)
			end = len;

	} else {
		/* Use len-1 by default, or if too big */
		if (start == SNEK_SOFFSET_NONE || start >= len)
			start = len - 1;

		/* Use -1 by default or if too small */
		if (end == SNEK_SOFFSET_NONE || end < -1)
			end = -1;
	}

	/* Compute the snek_slice_t value */
	snek_slice_t slice;

	slice.pos = start;
	slice.stride = stride;

	snek_soffset_t count = (end + stride - soffset_sgn(stride) - start) / stride;

	if (count < 0)
		count = 0;

	slice.count = count;
	slice.identity = start == 0 && count == len && stride == 1;

	/* Slice the compound object based on the computed snek_slice_t */
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
#endif

/*
 * Raise an undefined name exception
 */
static void
snek_undefined(snek_id_t id)
{
	snek_error("undefined: %s", snek_name_string(id));
}

/*
 * Perform assignment, both regular and enhanced (op=)
 */
static void
snek_assign(snek_id_t id, snek_op_t op)
{
	snek_poly_t *ref;

	/* For default formal values, don't override one
	 * passed by the caller
	 */
	if (op == snek_op_assign_named) {
		if (snek_id_is_local(id))
			return;
		op = snek_op_assign;
	}
	for (;;) {
		bool is_pure_assign = op == snek_op_assign;

		if (id != SNEK_ID_NONE) {
			/* don't create locals for enhanced assignment operators */
			if (!is_pure_assign && snek_frame && !snek_id_is_local(id))
				ref = NULL;
			else
				ref = snek_id_ref(id, is_pure_assign);
			if (!ref) {
				snek_undefined(id);
				return;
			}
		} else {
			/* Array operator assignment (a[x] = expr) */

			/* Fetch the index and list values off the stack */
			snek_poly_t ip = snek_stack_pop();
			snek_poly_t lp = snek_stack_pop();

			snek_list_t	*l;

			/* Make sure the array is a mutable list */
			if (snek_poly_type(lp) != snek_list ||
			    snek_list_readonly(l = snek_poly_to_list(lp)))
			{
				snek_error_type_1(lp);
				return;
			}

			/* Get a reference to the value location within the
			 * list
			 */
			ref = snek_list_ref(l, ip, true);
			if (!ref)
				return;
		}

		/* For simple assignment, we're done now */
		if (is_pure_assign)
			break;

		/* Recover the two values popped from the stack so
		 * that they will be popped again
		 */
		if (id == SNEK_ID_NONE)
			snek_stackp += 2;

		/* Go perform the binary operation to compute the
		 * final value. Note the conversion from enhanced
		 * assignment operator to regular binary operator
		 * here. This requires that the two sets of operators
		 * be in the same order
		 */
		snek_a = snek_binary(*ref, op - (snek_op_assign_plus - snek_op_plus), snek_a, true);

		/* Switch to a pure assignment now that the new value
		 * is known, then go re-compute the reference before
		 * storing (in case things have moved)
		 */
		op = snek_op_assign;
	}

	/* All done. Store the value at the computed location. */
	*ref = snek_a;
}

/*
 * Call a builtin function
 */
static void
snek_call_builtin(const snek_builtin_t *builtin, uint8_t nposition, uint8_t nnamed)
{
	snek_poly_t *actuals = &snek_stack[snek_stackp - (nposition + (nnamed << 1))];
	snek_soffset_t nformal = SNEK_BUILTIN_NFORMAL(builtin);

	/* Varargs functions have nformal == -1 */
	if (nformal < 0) {
		snek_a = SNEK_BUILTIN_FUNCV(builtin)(nposition, nnamed, actuals);
	} else if (nposition != nformal || nnamed) {

		/* Otherwise, complain if the argument count doesn't
		 * match. No builtins that have fixed arguments can be
		 * used with named actuals
		 */
		snek_error_args(nformal, nposition);
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
#if SNEK_BUILTIN_NAMES_MAX_ARGS >= 4
#error Need more builtin calls
#endif
		}
	}
}

/*
 * Execute code.
 *
 * This is the entry point for the snek virtual machine
 */
snek_poly_t
snek_exec(snek_code_t *code_in)
{
	snek_code = code_in;

	snek_poly_t	*ref = NULL;
	snek_id_t	id;
	snek_offset_t	ip = 0;
	snek_offset_t	o;
	snek_offset_t	saved_stackp = snek_stackp;

	/* Ending the top level code block will clear 'snek_code' to
	 * indicate completion
	 */
	while (snek_code) {

		/* Execute all of the instructions in the current code
		 * block
		 */
		while (ip < snek_code->size) {
#ifdef DEBUG_EXEC
			snek_code_dump_instruction(snek_code, ip);
#endif
			/* Pull out the next op code, note whether the
			 * 'push' flag is set and then figure out what
			 * to do
			 */
			snek_op_t op = snek_code->code[ip++];
			bool push = (op & snek_op_push) != 0;
			op &= ~snek_op_push;

			switch(op) {
			case snek_op_chain_eq:
			case snek_op_chain_ne:
			case snek_op_chain_gt:
			case snek_op_chain_lt:
			case snek_op_chain_ge:
			case snek_op_chain_le:
				op -= (snek_op_chain_eq - snek_op_eq);
				snek_poly_t r = snek_binary(snek_stack_pick(0), op, snek_a, false);
				snek_stack_drop(1);
				if (!snek_poly_true(r)) {
					snek_a = r;
					memcpy(&ip, &snek_code->code[ip], sizeof (snek_offset_t));
					push = false;
				} else
					ip += sizeof (snek_offset_t);
				break;
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
#ifndef SNEK_NO_DICT
			case snek_op_dict:
#endif
				memcpy(&o, &snek_code->code[ip], sizeof(snek_offset_t));
				ip += sizeof (snek_offset_t);
				snek_a = snek_list_imm(o, op - snek_op_list);
				break;
			case snek_op_id:
				memcpy(&id, &snek_code->code[ip], sizeof(snek_id_t));
				ip += sizeof (snek_id_t);
				ref = snek_id_ref(id, false);

				/* Allow re-definition of builtin names by looking
				 * to see if there is a value in the frame before
				 * checking for a builtin definition
				 */
				if (ref) {
					snek_a = *ref;
					break;
				}
				if (id < SNEK_BUILTIN_MAX_BUILTIN) {
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
				snek_a = snek_float_to_poly(~(uint32_t) snek_float_to_int(snek_poly_get_float(snek_a)));
				break;
			case snek_op_call:

				/* find out how many positional and named actuals were provided */
				memcpy(&o, &snek_code->code[ip], sizeof (snek_offset_t));
				snek_offset_t nposition = (o & 0xff);
				snek_offset_t nnamed = (o >> 8);

				/* Compute the number of stack values present; named
				 * actuals take two values (one name, one value)
				 */
				snek_offset_t nstack = nposition + (nnamed<<1);

				/* Go load the function value off the stack. snek_a isn't
				 * used for function calls, so we can save it here
				 */
				snek_a = snek_stack_pick(nstack);

				switch (snek_poly_type(snek_a)) {
				case snek_func:

					/* Arrange for the code in the function to run
					 * by creating a new frame
					 */
					if (!snek_func_push(nposition, nnamed, ip - 1))
						break;
					snek_a = snek_stack_pop();	/* get function back */

					/* Set our current code pointer and ip to point at the
					 * function's code
					 */
					snek_code = snek_pool_addr(snek_poly_to_func(snek_a)->code);
					ip = 0;
					push = false;	/* will pick up push on return */
					goto done_func;	/* skip ip and stack adjustment */
				case snek_builtin:

					/* Call the builtin function */
					snek_call_builtin(snek_poly_to_builtin(snek_a), nposition, nnamed);
					break;
				default:
					snek_error_type_1(snek_a);
					break;
				}
				/* Skip the parameter count in the bytecode */
				ip += sizeof (snek_offset_t);

				/* Drop all actuals */
				snek_stack_drop(nstack + 1);
			done_func:
				break;
			case snek_op_slice:
#ifdef SNEK_NO_SLICE
				snek_error_0("No slices");
#else
				snek_slice(snek_code->code[ip]);
#endif
				ip++;
				break;
			case snek_op_global:
				memcpy(&id, &snek_code->code[ip], sizeof (snek_id_t));
				ip += sizeof (snek_id_t);
				snek_frame_mark_global(id);
				break;
			case snek_op_del:
				memcpy(&id, &snek_code->code[ip], sizeof (snek_id_t));
				ip += sizeof (snek_id_t);

				if (id == SNEK_ID_NONE) {

					/* Delete an element from a list/dictionary */
					snek_poly_t lp = snek_stack_pop();
					if (snek_poly_type(lp) != snek_list) {
						snek_error_type_1(lp);
					} else {
						snek_list_del(lp, snek_a);
						snek_a = SNEK_NULL;
					}
				} else {

					/* Delete a name from the current scope */
					snek_id_del(id);
				}
				break;
			case snek_op_return:

				/* jump to the end of the current code block */
				ip = snek_code->size;
				break;
			case snek_op_assert:
				if (!snek_poly_true(snek_a)) {
					snek_error_0("AssertionError");
				}
				snek_a = SNEK_NULL;
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
				snek_error_0("not in loop");
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
			case snek_op_null:
				snek_a = SNEK_NULL;
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
			fprintf(stderr, "\t\ta= ");
			snek_poly_print(stderr, snek_a, 'r');
			for (o = snek_stackp; o;) {
				fprintf(stderr, ", [%d]= ", snek_stackp - o);
				snek_poly_print(stderr, snek_stack[--o], 'r');
			}
			fprintf(stderr, "\n");
#endif
		}

		/* Done with current code block. Pop the current frame and
		 * use the ip value saved there
		 */
		ip = snek_frame_pop();

		if (snek_code) {

			/* If we have another frame, push the accumulator if desired
			 * and step over the call instruction
			 */
			snek_op_t op = snek_code->code[ip];
			if ((op & snek_op_push) != 0)
				snek_stack_push(snek_a);
			ip += sizeof (snek_offset_t) + 1;
		}
	}
abort:
	/* Clear references to run objects */
	snek_code = NULL;
	snek_frame = NULL;
	snek_stackp = saved_stackp;
	snek_poly_t ret = snek_a;
	snek_a = SNEK_NULL;
	return ret;
}
