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

static snek_offset_t
snek_func_line(snek_func_t *func)
{
	snek_code_t	*code = snek_pool_addr(func->code);
	if (code)
		return snek_code_line(code);
	return 0;
}

void
snek_poly_format(snek_buf_t *buf, snek_poly_t a, char format)
{
	void *closure = buf->closure;
	static char tmp[32];
	char format_string[3] = "%.";
	snek_type_t atype = snek_poly_type(a);

	format_string[1] = format;
	format_string[1] = format;
	switch (format) {
	case 'd':
	case 'i':
	case 'u':
	case 'x':
	case 'X':
		if (atype != snek_float)
			break;
		sprintf(tmp, format_string, (int) snek_poly_to_float(a));
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
		sprintf(tmp, format_string, snek_poly_to_float(a));
		buf->put_s(tmp, closure);
		return;
	case 'c':
		switch (atype) {
		case snek_float:
			sprintf(tmp, format_string, (int) snek_poly_to_float(a));
			buf->put_s(tmp, closure);
			return;
		case snek_string:
			sprintf(tmp, format_string, (int) snek_poly_to_string(a)[0]);
			buf->put_s(tmp, closure);
			return;
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
		sprintf_const(tmp, "%.7g", snek_poly_to_float(a));
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
		buf->put_c(snek_list_readonly(list) ? '(' : '[', closure);
		snek_poly_t *data = snek_pool_addr(list->data);
		for (snek_offset_t o = 0; o < list->size; o++) {
			if (o)
				buf->put_c(' ', closure);
			snek_poly_format(buf, data[o], format);
			if (o < list->size - 1 || (list->size == 1 && snek_list_readonly(list)))
				buf->put_c(',', closure);
		}
		buf->put_c(snek_list_readonly(list) ? ')' : ']', closure);
		break;
	}
	}
}
