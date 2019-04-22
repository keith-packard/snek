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

#ifndef _SNEK_POSIX_H_
#define _SNEK_POSIX_H_

extern FILE	*snek_posix_input;

int snek_getc(FILE *input);

#ifdef __APPLE__
#define isnanf isnan
#endif

#define SNEK_GETC()	snek_getc(snek_posix_input)

#define SNEK_DEBUG	1

#endif /* _SNEK_POSIX_H_ */
