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

#include <ao.h>
#include <ao_snek.h>
#include "snek.h"

static bool	raw_mode;
static char	buf[RX_LINEBUF];
static uint8_t	used, avail;

void
ao_usb_out_hook(uint8_t *hook_buf, uint16_t len)
{
	uint16_t i;

	for (i = 0; i < len; i++)
		if (hook_buf[i] == ('c' & 0x1f))
			snek_abort = true;
}

static void
ao_snek_backspace(void)
{
	avail--;
	if (!raw_mode)
		fputs("\b \b", stdout);
}

static void
ao_snek_addc(char c)
{
	buf[avail++] = c;
	if (!raw_mode)
		putchar(c);
}

int
ao_snek_getchar(FILE *stream)
{
	if (used == avail) {
	restart:
		if (snek_parse_middle)
			putchar('+');
		else
			putchar('>');
		putchar(' ');
		used = avail = 0;
		for (;;) {
			fflush(stdout);
			uint8_t c = getc(stream);

			switch (c)
			{
			case '\r':
			case '\n':
				ao_snek_addc('\n');
				fflush(stdout);
				break;
			case 'n' & 0x1f:
				raw_mode = true;
				continue;
			case 'o' & 0x1f:
				raw_mode = false;
				continue;
			case 'c' & 0x1f:
				fputs("^C\n", stdout);
				snek_abort = false;
				goto restart;
			case 'h' & 0x1f:
			case 0x7f:
				if (avail)
					ao_snek_backspace();
				continue;
			case 'u' & 0x1f:
				while (avail)
					ao_snek_backspace();
				continue;
			case '\t':
				c = ' ';
				/* fall through ... */
			default:
				if (c >= (uint8_t)' ') {
					if (avail < RX_LINEBUF-1)
						ao_snek_addc(c);
				}
				continue;
			}
			break;
		}
	}
	return buf[used++];
}
