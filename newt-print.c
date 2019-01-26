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

#include "newt.h"

static inline newt_offset_t
newt_func_line(newt_func_t *func)
{
	newt_code_t	*code = newt_pool_ref(func->code);
	if (code)
		return newt_code_line(code);
	return 0;
}

void
newt_poly_format(newt_buf_t *buf, newt_poly_t a, char format)
{
	void *closure = buf->closure;
	static char tmp[32];
	char format_string[3] = "%.";
	newt_type_t atype = newt_poly_type(a);

	format_string[1] = format;
	format_string[1] = format;
	switch (format) {
	case 'd':
	case 'i':
	case 'u':
	case 'x':
	case 'X':
		if (atype != newt_float)
			break;
		sprintf(tmp, format_string, (int) newt_poly_to_float(a));
		buf->put_s(tmp, closure);
		return;
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
		if (atype != newt_float)
			break;
		sprintf(tmp, format_string, newt_poly_to_float(a));
		buf->put_s(tmp, closure);
		return;
	case 'c':
		switch (atype) {
		case newt_float:
			sprintf(tmp, format_string, (int) newt_poly_to_float(a));
			buf->put_s(tmp, closure);
			return;
		case newt_string:
			sprintf(tmp, format_string, (int) newt_poly_to_string(a)[0]);
			buf->put_s(tmp, closure);
			return;
		default:
			break;
		}
		break;
	case 's':
		if (atype == newt_string) {
			buf->put_s(newt_poly_to_string(a), closure);
			return;
		}
		break;
	default:
		break;
	}
	switch (atype) {
	case newt_float:
		sprintf_const(tmp, "%g", newt_poly_to_float(a));
		buf->put_s(tmp, closure);
		break;
	case newt_string:
		buf->put_c('\'', closure);
		buf->put_s(newt_poly_to_string(a), closure);
		buf->put_c('\'', closure);
		break;
	case newt_func:
		sprintf_const(tmp, "<function at %d>",
			      newt_func_line(newt_poly_to_func(a)));
		buf->put_s(tmp, closure);
		break;
	case newt_builtin:
		sprintf_const(tmp, "<builtin %s>",
			      newt_name_string(newt_poly_to_builtin_id(a)));
		buf->put_s(tmp, closure);
		break;
	case newt_list:
	{
		newt_list_t *list = newt_poly_to_list(a);
		buf->put_c(newt_list_readonly(list) ? '(' : '[', closure);
		newt_poly_t *data = newt_pool_ref(list->data);
		for (newt_offset_t o = 0; o < list->size; o++) {
			if (o)
				buf->put_c(' ', closure);
			newt_poly_format(buf, data[o], format);
			if (o < list->size - 1 || (list->size == 1 && newt_list_readonly(list)))
				buf->put_c(',', closure);
		}
		buf->put_c(newt_list_readonly(list) ? ')' : ']', closure);
		break;
	}
	}
}
