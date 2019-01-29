/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
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
#include <assert.h>

bool snek_print_vals;
bool snek_print_val;

#define MAX_FORMALS	10

static uint8_t nformal;
static snek_id_t formals[MAX_FORMALS];

snek_token_val_t snek_token_val;

#define GRAMMAR_TABLE
#include "snek-gram.h"

#ifndef VALUE_STACK_SIZE
#define VALUE_STACK_SIZE	128
#endif

static snek_token_val_t value_stack[VALUE_STACK_SIZE];
static int value_stack_p = 0;

static inline snek_token_val_t
value_get(int pos)
{
	return value_stack[value_stack_p - pos];
}

//#define VALUE_DEBUG

static inline snek_token_val_t
_value_pop(const char *file, int line)
{
	(void) file; (void) line;
#ifdef VALUE_DEBUG
	printf("value pop %d from %s:%d\n", value_stack_p - 1, file, line);
#endif
	if (value_stack_p == 0) {
		snek_error("value stack underflow");
		return snek_token_val;
	}
	return value_stack[--value_stack_p];
}

static inline bool
_value_push(snek_token_val_t value, const char *file, int line)
{
	(void) file; (void) line;
	if (value_stack_p >= VALUE_STACK_SIZE)
		return false;
#ifdef VALUE_DEBUG
	printf("\tpush %d from %s:%d\n", value_stack_p, file, line);
#endif
	value_stack[value_stack_p++] = value;
	return true;
}

static inline bool
_value_push_offset(snek_offset_t value, const char *file, int line)
{
	(void) file; (void) line;
#ifdef VALUE_DEBUG
	printf("value push offset %u\n", (unsigned) value);
#endif
	return _value_push((snek_token_val_t) { .offset = value }, file, line);
}

static inline bool
_value_push_bool(bool value, const char *file, int line)
{
	(void) file; (void) line;
#ifdef VALUE_DEBUG
	printf("value push bool %d\n", value);
#endif
	return _value_push((snek_token_val_t) { .bools = value }, file, line);
}

static inline bool
_value_push_id(snek_id_t id, const char *file, int line)
{
	(void) file; (void) line;
#ifdef VALUE_DEBUG
	printf("value push id %s\n", snek_name_string(id));
#endif
	return _value_push((snek_token_val_t) { .id = id }, file, line);
}

#define value_pop() _value_pop(__FILE__, __LINE__)
#define value_push(a) _value_push(a, __FILE__, __LINE__)
#define value_push_int(i) _value_push_int(i, __FILE__, __LINE__)
#define value_push_bool(b) _value_push_bool(b, __FILE__, __LINE__)
#define value_push_offset(o) _value_push_offset(o, __FILE__, __LINE__)
#define value_push_op(o) _value_push_op(o, __FILE__, __LINE__)
#define value_push_id(o) _value_push_id(o, __FILE__, __LINE__)

#ifndef PARSE_STACK_SIZE
#define PARSE_STACK_SIZE 256
#endif

static inline token_t
lex(void *lex_context)
{
	bool *skip_to_nl = lex_context;

	if (*skip_to_nl) {
		token_t token;
		*skip_to_nl = false;
		for (;;) {
			token = snek_lex();
			switch(token) {
			case END:
				return END;
			case NL:
				if (snek_current_indent == 0)
					break;
				continue;
			case EXDENT:
				snek_current_indent = snek_token_val.indent;
				continue;
			default:
				continue;
			}
			break;
		}
	}
	return snek_lex();
}

#define PARSE_ACTION_BOTTOM do {			\
		if (snek_abort)				\
			return parse_return_error;	\
	} while (0)

#define PARSE_CODE
#include "snek-gram.h"

snek_parse_ret_t
snek_parse(void)
{
	bool skip_to_nl = false;
	snek_line = 1;

	for (;;) {
		snek_current_indent = 0;
		snek_ignore_nl = 0;
		snek_abort = false;
		value_stack_p = 0;

		parse_return_t ret = parse(&skip_to_nl);

		switch (ret) {
		case parse_return_success:
			return snek_parse_success;
		case parse_return_end:
			snek_error("Syntax error at end of file.");
			return snek_parse_error;
		case parse_return_syntax:
			snek_error("Syntax error at \"%s\".", snek_lex_text);
			skip_to_nl = true;
			break;
		case parse_return_error:
		case parse_return_oom:
			break;
		default:
			skip_to_nl = true;
			break;
		}
	}
}
