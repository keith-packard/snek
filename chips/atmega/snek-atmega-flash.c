/*
 * Copyright © 2024 Keith Packard <keithp@keithp.com>
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
#include <avr/io.h>
#include <stddef.h>

static uint8_t		*flash_write_addr;
static bool		flash_write_page_pending;

static inline uint8_t *
flash_start(void)
{
	uint8_t	append = FUSE_APPEND;

	if (append == 0)
		return NULL;
	return (uint8_t *) (MAPPED_PROGMEM_START + append * 256);
}

static inline uint8_t *
flash_end(void)
{
	return (uint8_t *) MAPPED_PROGMEM_END;
}

static inline ptrdiff_t
flash_size(void)
{
	uint8_t *start = flash_start();
	if (!start)
		return 0;
	return (flash_end() - start) + 1;
}

static inline bool
flash_start_of_page(uint8_t *addr) {
	return ((uintptr_t) addr & (PROGMEM_PAGE_SIZE - 1)) == 0;
}

static inline bool
flash_end_of_page(uint8_t *addr) {
	return (((uintptr_t) addr + 1) & (PROGMEM_PAGE_SIZE - 1)) == 0;
}

static inline uint8_t *
flash_page_addr(uint8_t *addr) {
	return (uint8_t *) ((uintptr_t) addr & ~(PROGMEM_PAGE_SIZE - 1));
}

static bool
flash_write_full(void)
{
	return flash_write_addr == flash_end();
}

static void
flash_cmd(uint8_t cmd)
{
	while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm|NVMCTRL_EEBUSY_bm));
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, cmd);
	while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm|NVMCTRL_EEBUSY_bm));
}

static void
flash_write_init(void)
{
	flash_write_addr = flash_start();
	flash_write_page_pending = false;
	flash_cmd(NVMCTRL_CMD_PAGEBUFCLR_gc);
}

static void
flash_write_flush(void)
{
	if (flash_write_page_pending) {
		flash_write_page_pending = false;
		/*
		 * Wait for an ENQ to appear in the input buffer. Once
		 * that is there, the sender will stop transmitting
		 * until we reply with an ACK, and that won't happen
		 * until we read the ENQ from the input buffer, so we
		 * can safely stop the CPU while writing flash without
		 * dropping any incoming data. Give up if not seen within
		 * a few dozen ms; that will happen when the sender is
		 * done sending commands.
		 */
		snek_uart_wait_queued('e' & 0x1f, U_TICKS_PER_SECOND / 16);
		flash_cmd(NVMCTRL_CMD_PAGEERASEWRITE_gc);
	}
}

static void
flash_write_byte(uint8_t byte)
{
	*flash_write_addr = byte;
	flash_write_page_pending = true;
	if (flash_end_of_page(flash_write_addr))
		flash_write_flush();
	flash_write_addr++;
}

static uint8_t
flash_read_byte(uint8_t *addr)
{
	return *addr;
}

snek_poly_t
snek_builtin_eeprom_write(void)
{
	uint8_t 	c;

	flash_write_init();
	for(;;) {
		c = snek_raw_getc(stdin);
		if (c == ('d' & 0x1f))
			c = 0xff;
		flash_write_byte(c);
		if (flash_write_full())
			break;
		if (c == 0xff)
			break;
	}
	flash_write_flush();
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_eeprom_show(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	uint8_t 	c;
	uint8_t		*addr;

	(void) nnamed;
	(void) args;
	if (nposition)
		putc('b' & 0x1f, stdout);
	for (addr = flash_start(); addr <= flash_end(); addr++) {
		c = flash_read_byte(addr);
		if (c == 0xff)
			break;
		putc(c, stdout);
	}
	if (nposition)
		putc('c' & 0x1f, stdout);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_eeprom_erase(void)
{
	if (flash_read_byte(0) != 0xff) {
		flash_write_init();
		flash_write_byte(0xff);
		flash_write_flush();
	}
	return SNEK_NULL;
}

static ptrdiff_t		snek_flash_off;

int
snek_eeprom_getchar(FILE *stream)
{
	(void) stream;
	if (snek_flash_off < flash_size() && !snek_abort) {
		uint8_t c = flash_read_byte(flash_start() + snek_flash_off++);
		if (c != 0xff)
			return c;
	}
	snek_interactive = true;
	snek_duino_file.get = snek_io_getc;
	return EOF;
}
