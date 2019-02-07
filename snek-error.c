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

snek_poly_t
snek_error_name(const char *format, ...)
{
	va_list		args;
	char		c;

	snek_abort = true;
	va_start(args, format);
	fprintf(stderr, "%s:%d ", snek_file, snek_line);
	while ((c = ERROR_FETCH_FORMAT_CHAR(format++))) {
		if (c == '%') {
			switch ((c = ERROR_FETCH_FORMAT_CHAR(format++))) {
#if 0
			case '\0':
				--format;
				break;
			case '%':
				putc(c, stderr);
				break;
#endif
			case 'd':
				fprintf(stderr, "%d", va_arg(args, int));
				break;
#if 0
			case 'x':
				fprintf(stderr, "%x", va_arg(args, int));
				break;
			case 'i':
				fprintf(stderr, "%s", snek_name_string(va_arg(args, int)));
				break;
			case 'g':
				fprintf(stderr, "%.7g", va_arg(args, double));
				break;
			case 'S':
				len = va_arg(args, int);
				fwrite(va_arg(args, char *), 1, len, stderr);
				break;
#endif
			case 's':
				fputs(va_arg(args, char *), stderr);
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
	return SNEK_ZERO;
}

snek_poly_t
snek_error_range(snek_soffset_t o)
{
	return snek_error("index out of range: %d", o);
}

#if SNEK_DEBUG
void
snek_panic(const char *message)
{
	snek_error("%s\n", message);
	abort();
}
#endif
