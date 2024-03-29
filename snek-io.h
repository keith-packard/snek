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

#ifndef _SNEK_IO_H_
#define _SNEK_IO_H_

#include <stdio.h>

#ifndef SNEK_IO_PUTS
#define SNEK_IO_PUTS(s)	fputs(s, stdout)
#endif
#ifndef SNEK_IO_PUTC
#define SNEK_IO_PUTC(c) putchar(c)
#endif
#ifndef SNEK_IO_WAITING
#define SNEK_IO_WAITING(s) false
#endif
#ifndef SNEK_IO_LINEBUF
#define SNEK_IO_LINEBUF	132
#endif
#ifndef SNEK_IO_GETC
#define SNEK_IO_GETC(s) getchar()
#endif

int
snek_io_getc(FILE *stream);

static inline char
snek_raw_getc(FILE *stream)
{
	char c;
	if (!SNEK_IO_WAITING(stream))
		fflush(stdout);
	c = SNEK_IO_GETC(stream);
	if (c == ('e' & 0x1f)) {
		SNEK_IO_PUTC('f' & 0x1f);
		return snek_raw_getc(stream);
	}
	if (c == '\r')
		c = '\n';
	return c;
}

#endif /* _SNEK_IO_H_ */
