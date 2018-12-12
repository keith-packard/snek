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

#include "newt.h"

bool newt_abort;

void
newt_error(char *format, ...)
{
	va_list		args;
	char		c;
	int		len;

	newt_abort = true;
	va_start(args, format);
	fprintf(stderr, "%s:%d ", newt_file, newt_line);
	while ((c = *format++)) {
		if (c == '%') {
			switch ((c = *format++)) {
			case '\0':
				--format;
				break;
			case '%':
				putc(c, stderr);
				break;
			case 'd':
				fprintf(stderr, "%d", va_arg(args, int));
				break;
			case 'i':
				fprintf(stderr, "%s", newt_name_string(va_arg(args, int)));
				break;
			case 'g':
				fprintf(stderr, "%g", va_arg(args, double));
				break;
			case 'S':
				len = va_arg(args, int);
				fwrite(va_arg(args, char *), 1, len, stderr);
				break;
			case 's':
				fputs(va_arg(args, char *), stderr);
				break;
			case 'p':
				newt_poly_print(stderr, va_arg(args, newt_poly_t));
				break;
			}
		} else
			putc(c, stderr);
	}
	va_end(args);
}
