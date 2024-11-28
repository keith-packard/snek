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

bool snek_abort;

static void
puts_clean(char *s)
{
	unsigned char c;
	while ((c = (unsigned char) *s++)) {
		if (c < ' ') {
			putc('\\', stderr);
			switch (c) {
			case '\n':
				putc('n', stderr);
				break;
			default:
				fprintf(stderr, "x%02x", c);
				break;
			}
		} else
			putc(c, stderr);
	}
}

snek_poly_t
snek_error_name(CONST char *format, ...)
{
	va_list		args;
	char		c;

	if (snek_abort)
		return SNEK_NULL;
	snek_abort = true;
	va_start(args, format);
#ifdef SNEK_NO_FILE
	fprintf(stderr, "<stdin>:%d ", snek_line);
#else
	fprintf(stderr, "%s:%d ", snek_file, snek_line);
#endif
	while ((c = *format++)) {
		if (c == '%') {
			switch ((c = *format++)) {
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
snek_error_0_name(CONST char *string)
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
	return snek_error_0("zero step");
}

snek_poly_t
snek_error_args(snek_soffset_t want, snek_soffset_t got)
{
	return snek_error("bad args. wanted %d got %d", want, got);
}

snek_poly_t
snek_error_arg(snek_id_t bad)
{
	return snek_error("bad arg \"%s\"", snek_name_string(bad));
}

snek_poly_t
snek_error_syntax(char *where)
{
	return snek_error("Syntax error at \"%s\".", where);
}

#if SNEK_DEBUG || defined(DEBUG_MEMORY)
void
snek_panic(const char *message)
{
	snek_error("%s", message);
	abort();
}
#endif
