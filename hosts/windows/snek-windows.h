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

#ifndef _SNEK_WINDOWS_H_
#define _SNEK_WINDOWS_H_

extern FILE	*snek_windows_input;

int snek_getc(FILE *input);

#define strfromf(dst, len, fmt, val) sprintf_const(dst, fmt, val)

#define SNEK_GETC()	snek_getc(snek_windows_input)

#define isnanf(x) __isnanf(x)

#define CONST const

#endif /* _SNEK_WINDOWS_H_ */
