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

#include "snek.h"
#include "snek-io.h"

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

static bool	raw_mode;
static char	buf[SNEK_IO_LINEBUF];
static uint8_t	used, avail;

static void
snek_io_backspace(void)
{
	avail--;
	if (!raw_mode)
		SNEK_IO_PUTS("\b \b");
}

static void
snek_io_addc(char c)
{
	buf[avail++] = c;
	if (!raw_mode)
		SNEK_IO_PUTC(c);
}

#ifdef SNEK_BUILTIN_input
extern bool snek_in_input;
static char unget;
#endif

int
snek_io_getc(FILE *stream)
{
	(void) stream;
	if (used == avail) {
#ifdef SNEK_BUILTIN_input
		if (!snek_in_input)
#endif
		{
	restart_cooked:
			if (snek_parse_middle)
				SNEK_IO_PUTC('+');
			else
				SNEK_IO_PUTC('>');
			SNEK_IO_PUTC(' ');
		}
	restart_raw:
		used = avail = 0;
		for (;;) {
			if (!SNEK_IO_WAITING(stream))
				fflush(stdout);
			uint8_t c;

#ifdef SNEK_BUILTIN_input
			if (unget) {
				c = unget;
				unget = 0;
			} else
#endif
			c = SNEK_IO_GETC(stream);

			switch (c)
			{
			case '\r':
			case '\n':
				snek_io_addc('\n');
				break;
			case 'n' & 0x1f:
				raw_mode = true;
				continue;
			case 'o' & 0x1f:
				raw_mode = false;
				continue;
			case 'c' & 0x1f:
#ifdef SNEK_BUILTIN_input
				if (snek_in_input) {
					unget = c;
					return EOF;
				}
#endif
				snek_abort = false;
				if (raw_mode)
					goto restart_raw;
				SNEK_IO_PUTS("^C\n");
				goto restart_cooked;
			case 'h' & 0x1f:
			case 0x7f:
				if (avail)
					snek_io_backspace();
				continue;
			case 'u' & 0x1f:
				while (avail)
					snek_io_backspace();
				continue;
			case '\t':
				c = ' ';
				/* fall through ... */
			default:
				if (c >= (uint8_t)' ') {
					if (avail < SNEK_IO_LINEBUF-1)
						snek_io_addc(c);
				}
				continue;
			}
			break;
		}
	}
	return buf[used++];
}
