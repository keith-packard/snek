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

FILE snek_avr_file = FDEV_SETUP_STREAM(ao_usb_putc, snek_eeprom_getchar, _FDEV_SETUP_RW);

static void
snek_intflash_wait_idle(void)
{
	/* Wait for any outstanding writes to complete */
	while (EECR & (1 << EEPE))
			;
}

static void
snek_intflash_write(snek_offset_t pos, uint8_t d)
{
	EEAR = pos;
	EEDR = d;
	ao_arch_critical(
		EECR |= (1 << EEMPE);
		EECR |= (1 << EEPE);
		);
	snek_intflash_wait_idle();
}

static uint8_t
snek_intflash_read(snek_offset_t pos)
{
	EEAR = pos;
	EECR |= (1 << EERE);
	return EEDR;
}

snek_poly_t
snek_builtin_eeprom_write(void)
{
	uint8_t c;
	snek_offset_t	addr = 0;

	for (addr = 0; addr < 1024; addr++) {
		c = ao_usb_getc();
		if (c == '\r')
			c = '\n';
		if (c == ('d' & 0x1f))
			c = 0xff;
		snek_intflash_write(addr, c);
		if (c == 0xff)
			break;
	}
	return SNEK_NULL;
}

static snek_soffset_t	snek_eeprom_addr;

#if 0
snek_poly_t
snek_builtin_eeprom_load(void)
{
	snek_interactive = false;
	snek_eeprom_addr = 0;
	snek_avr_file.get = snek_eeprom_getchar;
	return SNEK_NULL;
}
#endif

static int __attribute__((noinline))
eeprom_showc(char c)
{
	return putc(c, stdout);
}

static uint8_t __attribute__((noinline))
eeprom_getc(void)
{
	if (snek_eeprom_addr < 1024)
		return snek_intflash_read(snek_eeprom_addr++);
	return 0xff;
}

snek_poly_t
snek_builtin_eeprom_show(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	uint8_t c;

	(void) nnamed;
	(void) args;
	if (nposition)
		eeprom_showc('b' & 0x1f);
	snek_eeprom_addr = 0;
	while ((c = eeprom_getc()) != 0xff)
		eeprom_showc(c);
	if (nposition)
		eeprom_showc('c' & 0x1f);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_eeprom_erase(void)
{
	if (snek_intflash_read(0) != 0xff)
		snek_intflash_write(0, 0xff);
	return SNEK_NULL;
}

int
snek_eeprom_getchar(FILE *stream)
{
	(void) stream;
	uint8_t c = eeprom_getc();
	if (c != 0xff)
		return c;
	snek_interactive = true;
	snek_avr_file.get = snek_io_getc;
	return EOF;
}
