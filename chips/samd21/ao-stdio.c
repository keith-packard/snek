/*
 * Copyright © 2009 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include "ao.h"
#include "snek.h"
#include "snek-io.h"

static FILE __stdio = FDEV_SETUP_STREAM(ao_usb_putc, snek_io_getc, ao_usb_flush, _FDEV_SETUP_RW);

#ifdef __strong_reference
#define STDIO_ALIAS(x) __strong_reference(stdin, x);
#else
#define STDIO_ALIAS(x) FILE *const x = &__stdio;
#endif

FILE *const stdin = &__stdio;
STDIO_ALIAS(stdout);
STDIO_ALIAS(stderr);

#ifdef _SNEK_NEED_STRFROMF
int
strfromf(char *__restrict str, size_t n,
	 const char *__restrict format, float fp)
{
	return snprintf(str, n, format, __printf_float(fp));
}
#endif


#ifdef _SNEK_NEED_STRTOF
float
strtof(const char *restrict nptr, char **restrict endptr)
{
	float f;
	(void) sscanf(nptr, "%f", &f);
	(void) endptr;
	return f;
}
#endif
