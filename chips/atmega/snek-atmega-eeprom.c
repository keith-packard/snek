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
#include <avr/eeprom.h>

#ifndef EEPROM_SIZE
#define EEPROM_SIZE (E2END + 1)
#endif

snek_poly_t
snek_builtin_eeprom_write(void)
{
	uint8_t c;
	snek_offset_t	addr = 0;

	for(;;) {
		c = snek_raw_getc(stdin);
		if (c == ('d' & 0x1f))
			c = 0xff;
		eeprom_write_byte((uint8_t *) addr, c);
		addr++;
		if (addr == EEPROM_SIZE)
			break;
		if (c == 0xff)
			break;
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_eeprom_show(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	uint8_t c;
	snek_offset_t	addr = 0;

	(void) nnamed;
	(void) args;
	if (nposition)
		putc('b' & 0x1f, stdout);
	for (addr = 0; addr < EEPROM_SIZE; addr++) {
		c = eeprom_read_byte((uint8_t *) addr);
		if (c == 0xff)
			break;
		putc(c, stdout);
	}
	if (nposition)
		putc('c' & 0x1f, stdout);
	return SNEK_NULL;
}

static snek_offset_t	snek_eeprom_addr;

snek_poly_t
snek_builtin_eeprom_erase(void)
{
	if (eeprom_read_byte((uint8_t *) 0) != 0xff)
		eeprom_write_byte((uint8_t *) 0, 0xff);
	return SNEK_NULL;
}

int
snek_eeprom_getchar(FILE *stream)
{
	(void) stream;
	if (snek_eeprom_addr < EEPROM_SIZE && !snek_abort) {
		uint8_t c = eeprom_read_byte((uint8_t *) (snek_eeprom_addr++));
		if (c != 0xff)
			return c;
	}
	snek_interactive = true;
	snek_duino_file.get = snek_io_getc;
	return EOF;
}
