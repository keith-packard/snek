/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
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

static snek_offset_t
snek_func_line(snek_func_t *func)
{
	snek_code_t	*code = snek_pool_addr(func->code);
	if (code)
		return snek_code_line(code);
	return 0;
}

static inline char snek_list_open(snek_list_type_t type)
{
	switch(type) {
	case snek_list_list:
		return '[';
	default:
#ifndef SNEK_NO_DICT
		return '{';
	case snek_list_tuple:
#endif
		return '(';
	}
}

static inline char snek_list_close(snek_list_type_t type)
{
	switch(type) {
	case snek_list_list:
		return ']';
	default:
#ifndef SNEK_NO_DICT
		return '}';
	case snek_list_tuple:
#endif
		return ')';
	}
}

#ifndef printf_float
#define printf_float(x) ((double) (x))
#endif

void
snek_poly_format(snek_buf_t *buf, snek_poly_t a, char format)
{
	void *closure = buf->closure;
	static char tmp[32];
	static char format_string[3] = "%.";
	snek_type_t atype = snek_poly_type(a);
	int i;

	format_string[1] = format;
	switch (format) {
	case 'd':
	case 'i':
	case 'o':
	case 'x':
	case 'X':
		if (atype != snek_float)
			break;
	print_float:
		i = (int) snek_poly_to_float(a);
	print_int:
		sprintf(tmp, format_string, i);
		buf->put_s(tmp, closure);
		return;
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
		if (atype != snek_float)
			break;
		strfromf(tmp, sizeof(tmp), format_string, snek_poly_to_float(a));
		buf->put_s(tmp, closure);
		return;
	case 'c':
		switch (atype) {
		case snek_float:
			goto print_float;
		case snek_string:
			i = snek_poly_to_string(a)[0];
			goto print_int;
		default:
			break;
		}
		break;
	case 's':
		if (atype == snek_string) {
			buf->put_s(snek_poly_to_string(a), closure);
			return;
		}
		break;
	default:
		break;
	}
	if (snek_is_null(a))
		buf->put_s("None", closure);
	else switch (atype) {
	case snek_float:
		strfromf_const(tmp, sizeof(tmp), "%.9g", snek_poly_to_float(a));
		buf->put_s(tmp, closure);
		break;
	case snek_string:
		buf->put_c('\'', closure);
		buf->put_s(snek_poly_to_string(a), closure);
		buf->put_c('\'', closure);
		break;
	case snek_func:
		sprintf_const(tmp, "<function at %d>",
			      snek_func_line(snek_poly_to_func(a)));
		buf->put_s(tmp, closure);
		break;
	case snek_builtin:
		sprintf_const(tmp, "<builtin %s>",
			      snek_name_string(snek_poly_to_builtin_id(a)));
		buf->put_s(tmp, closure);
		break;
	case snek_list:
	{
		snek_list_t *list = snek_poly_to_list(a);
		snek_list_type_t type = snek_list_type(list);
		snek_offset_t size = list->size;

		snek_stack_push_list(list);
		buf->put_c(snek_list_open(type), closure);
		for (snek_offset_t o = 0; o < size; o++) {
			list = snek_stack_pop_list();
			snek_stack_push_list(list);
			snek_poly_format(buf, snek_list_data(list)[o], format);
#ifdef SNEK_NO_DICT
#define list_sep_char(type,o) ','
#else
#define list_sep_char(type,o) (((type) == snek_list_dict && !((o)&1)) ? ':' : ',')
#endif
			if (o < size - 1 || (size == 1 && type == snek_list_tuple)) {
				buf->put_c(list_sep_char(type, o), closure);
				buf->put_c(' ', closure);
			}
		}
		list = snek_stack_pop_list();
		buf->put_c(snek_list_close(type), closure);
		break;
	}
	}
}
