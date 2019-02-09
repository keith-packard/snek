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
static bool snek_print_val;

uint8_t snek_parse_nformal;
snek_id_t snek_parse_formals[SNEK_MAX_FORMALS];

snek_token_val_t snek_token_val;

#define GRAMMAR_TABLE
#include "snek-gram.h"

#ifndef VALUE_STACK_SIZE
#define VALUE_STACK_SIZE	127
#endif

#if VALUE_STACK_SIZE < 128
typedef int8_t snek_value_stack_p_t;
#else
typedef int16_t snek_value_stack_p_t;
#endif

static snek_token_val_t value_stack[VALUE_STACK_SIZE];

static snek_value_stack_p_t value_stack_p = 0;

static uint8_t for_depth;

//#define VALUE_DEBUG

static snek_token_val_t
_value_pop(const char *file, int line)
{
	(void) file; (void) line;
#ifdef VALUE_DEBUG
	printf("value pop %d from %s:%d\n", value_stack_p - 1, file, line);
#endif
#if SNEK_DEBUG
	if (value_stack_p == 0) {
		snek_error("value stack underflow");
		return snek_token_val;
	}
#endif
	return value_stack[--value_stack_p];
}

static bool
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

static bool
_value_push_offset(snek_offset_t value, const char *file, int line)
{
	(void) file; (void) line;
#ifdef VALUE_DEBUG
	printf("value push offset %u\n", (unsigned) value);
#endif
	return _value_push((snek_token_val_t) { .offset = value }, file, line);
}

static bool
_value_push_bool(bool value, const char *file, int line)
{
	(void) file; (void) line;
#ifdef VALUE_DEBUG
	printf("value push bool %d\n", value);
#endif
	return _value_push((snek_token_val_t) { .bools = value }, file, line);
}

static bool
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
#define value_push_id(o) _value_push_id(o, __FILE__, __LINE__)

#ifndef PARSE_STACK_SIZE
#define PARSE_STACK_SIZE 128
#endif

#define lex(context) ({ (void) context; snek_lex(); })

#define PARSE_ACTION_BOTTOM do {			\
		if (snek_abort)				\
			return parse_return_error;	\
	} while (0)

#define PARSE_CODE
#include "snek-gram.h"

snek_parse_ret_t
snek_parse(void)
{
	snek_line = 1;

	for (;;) {
		snek_current_indent = 0;
		snek_ignore_nl = 0;
		snek_abort = false;
		value_stack_p = 0;
		for_depth = 0;

		parse_return_t ret = parse(NULL);

		switch (ret) {
		case parse_return_success:
			return snek_parse_success;
		case parse_return_end:
			snek_error("Syntax error at end of file.");
			return snek_parse_error;
		case parse_return_error:
		case parse_return_oom:
			break;
		case parse_return_syntax:
		default:
			snek_error("Syntax error at \"%s\".", snek_lex_text);
			{
				token_t token;
				snek_ignore_nl = 0;
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
			break;
		}
	}
}
