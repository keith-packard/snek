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

#include "snek.h"

bool snek_abort;

#ifndef ERROR_FETCH_FORMAT_CHAR
#define ERROR_FETCH_FORMAT_CHAR(a) (*(a))
#endif

static void
puts_clean(char *s)
{
	unsigned char c;
	while ((c = (unsigned char) *s++)) {
		if (c < ' ')
			fprintf(stderr, "\\x%02x", c);
		else
			putc(c, stderr);
	}
}

snek_poly_t
snek_error_name(const char *format, ...)
{
	va_list		args;
	char		c;

	if (snek_abort)
		return SNEK_NULL;
	snek_abort = true;
	va_start(args, format);
	fprintf(stderr, "%s:%d ", snek_file, snek_line);
	while ((c = ERROR_FETCH_FORMAT_CHAR(format++))) {
		if (c == '%') {
			switch ((c = ERROR_FETCH_FORMAT_CHAR(format++))) {
			case 'd':
				fprintf(stderr, "%d", va_arg(args, int));
				break;
			case 's':
				puts_clean(va_arg(args, char *));
				break;
			case 'p':
				snek_poly_print(stderr, va_arg(args, snek_poly_t), 'r');
				break;
#if SNEK_DEBUG
			default:
				snek_panic("bad snek_error format");
				break;
#endif
			}
		} else
			putc(c, stderr);
	}
	putc('\n', stderr);
	va_end(args);
	return SNEK_NULL;
}

snek_poly_t
snek_error_0_name(const char *string)
{
	return snek_error_name(string);
}

snek_poly_t
snek_error_value(snek_poly_t p)
{
	return snek_error("invalid value: %p", p);
}

snek_poly_t
snek_error_type_2(snek_poly_t a, snek_poly_t b)
{
	return snek_error("type mismatch: %p %p", a, b);
}

snek_poly_t
snek_error_type_1(snek_poly_t a)
{
	return snek_error("invalid type: %p", a);
}

snek_poly_t
snek_error_step(void)
{
	return snek_error("zero step");
}

#if SNEK_DEBUG
void
snek_panic(const char *message)
{
	snek_error(message);
	abort();
}
#endif
