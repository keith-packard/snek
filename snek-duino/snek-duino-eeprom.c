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

#include "snek.h"
#include <avr/eeprom.h>

snek_poly_t
snek_builtin_eeprom_write(void)
{
	uint8_t c;
	snek_offset_t	addr = 0;

	for (addr = 0; addr < 1024; addr++) {
		c = snek_uart_getch();
		if (c == '\r')
			c = '\n';
		if (c == ('d' & 0x1f))
			c = 0xff;
		eeprom_write_byte((uint8_t *) addr, c);
		if (c == 0xff)
			break;
	}
	return SNEK_ZERO;
}

snek_poly_t
snek_builtin_eeprom_show(void)
{
	uint8_t c;
	snek_offset_t	addr = 0;

	for (addr = 0; addr < 1024; addr++) {
		c = eeprom_read_byte((uint8_t *) addr);
		if (c == 0xff)
			break;
		putc(c, stdout);
	}
	return SNEK_ZERO;
}

snek_poly_t
snek_builtin_eeprom_load(void)
{
	snek_print_vals = false;
	snek_duino_file.get = snek_eeprom_getchar;
	return SNEK_ZERO;
}

snek_poly_t
snek_builtin_eeprom_erase(void)
{
	if (eeprom_read_byte((uint8_t *) 0) != 0xff)
		eeprom_write_byte((uint8_t *) 0, 0xff);
	return SNEK_ZERO;
}

static snek_soffset_t	snek_eeprom_addr;

int
snek_eeprom_getchar(FILE *stream)
{
	(void) stream;
	if (snek_eeprom_addr < 1024) {
		uint8_t c = eeprom_read_byte((uint8_t *) (snek_eeprom_addr++));
		if (c != 0xff)
			return c;
	}
	snek_print_vals = true;
	snek_duino_file.get = snek_uart_getchar;
	return snek_uart_getchar(NULL);
}
