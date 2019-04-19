/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
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

#include <ao.h>
#include <ao-flash.h>

/* Erase rows are four pages */
static uint32_t
samd21_nvmctrl_row_size(void)
{
	return samd21_nvmctrl_page_size() * 4;
}

/* size of a lock region. That's just total flash size / 16 */
static uint32_t
samd21_nvmctrl_lock_region(void)
{
	return samd21_flash_size() >> 4;
}

/* Find the bit index of an address within the lock word */
static uint8_t
ao_flash_lock_region_bit(void *addr)
{
	return ((uint32_t) addr) / samd21_nvmctrl_lock_region();
}

static uint8_t
ao_flash_is_locked(void *addr)
{
	return (samd21_nvmctrl.lock >> ao_flash_lock_region_bit(addr)) & 1;
}

/* Execute a single flash operation, waiting for it to complete. This
 * bit of code must be in ram
 */
static void __attribute__ ((section(".ramtext"),noinline))
_ao_flash_execute(uint16_t cmd)
{
	while ((samd21_nvmctrl.intflag & (1 << SAMD21_NVMCTRL_INTFLAG_READY)) == 0)
		;
	samd21_nvmctrl.ctrla = ((cmd << SAMD21_NVMCTRL_CTRLA_CMD) |
				(SAMD21_NVMCTRL_CTRLA_CMDEX_KEY << SAMD21_NVMCTRL_CTRLA_CMDEX));
	while ((samd21_nvmctrl.intflag & (1 << SAMD21_NVMCTRL_INTFLAG_READY)) == 0)
		;
	samd21_nvmctrl.intflag = ((1 << SAMD21_NVMCTRL_INTFLAG_READY) |
				  (1 << SAMD21_NVMCTRL_INTFLAG_ERROR));
}

/* Set the address of the next flash operation */
static void
_ao_flash_set_addr(void *addr)
{
	while ((samd21_nvmctrl.intflag & (1 << SAMD21_NVMCTRL_INTFLAG_READY)) == 0)
		;
	samd21_nvmctrl.addr = ((uint32_t) addr) >> 1;
	while ((samd21_nvmctrl.intflag & (1 << SAMD21_NVMCTRL_INTFLAG_READY)) == 0)
		;
}

/* Unlock a region of flash */
static void
_ao_flash_unlock(void *addr)
{
	if (!ao_flash_is_locked(addr))
		return;

	_ao_flash_set_addr(addr);
	_ao_flash_execute(SAMD21_NVMCTRL_CTRLA_CMD_UR);
}

#if 0
/* Lock a region of flash */
static void
_ao_flash_lock(void *addr)
{
	if (ao_flash_is_locked(addr))
		return;

	_ao_flash_set_addr(addr);
	_ao_flash_execute(SAMD21_NVMCTRL_CTRLA_CMD_LR);
}
#endif

/* Erase a row of flash */
static void
_ao_flash_erase_row(void *row)
{
	_ao_flash_unlock(row);
	_ao_flash_set_addr(row);
	_ao_flash_execute(SAMD21_NVMCTRL_CTRLA_CMD_ER);
}

static uint32_t	flash_write_offset;
static uint8_t	flash_write_byte;
static bool	flash_write_pending;

extern uint16_t	__flash__[];
extern uint16_t __flash_end__[];

#define __flash8__	((uint8_t *) __flash__)
#define __flash8_end__	((uint8_t *) __flash_end__)

static uint32_t ao_flash_size(void)
{
	return __flash8_end__ - __flash8__;
}

static bool
_ao_flash_row_erased(void *dst)
{
	uint8_t *d = dst;
	uint32_t offset;
	uint32_t row_size = samd21_nvmctrl_row_size();

	for (offset = 0; offset < row_size; offset++)
		if (d[offset] != 0xff)
			return false;
	return true;
}


static void
ao_flash_start_page(void *dst)
{
	fflush(stdout);
	ao_arch_block_interrupts();

	if (((uintptr_t) dst & (samd21_nvmctrl_row_size() - 1)) == 0) {
		_ao_flash_unlock(dst);
		if (!_ao_flash_row_erased(dst))
			_ao_flash_erase_row(dst);
	}

	/* Clear write buffer */
	_ao_flash_execute(SAMD21_NVMCTRL_CTRLA_CMD_PBC);

	ao_arch_release_interrupts();
}

static void
ao_flash_finish_page(void *dst)
{
	fflush(stdout);
	ao_arch_block_interrupts();

	_ao_flash_set_addr(dst);
	_ao_flash_execute(SAMD21_NVMCTRL_CTRLA_CMD_WP);

	ao_arch_release_interrupts();
}

void
ao_flash_write_init(void)
{
	flash_write_offset = 0;
	flash_write_pending = false;
}

void
ao_flash_write_byte(uint8_t c)
{
	/* Check for start page */
	if (flash_write_offset % samd21_nvmctrl_page_size() == 0)
		ao_flash_start_page(__flash8__ + flash_write_offset);

	/* write the byte */
	if (flash_write_pending) {
		__flash__[flash_write_offset >> 1] = ((uint16_t) c << 8) | flash_write_byte;
		flash_write_offset += 2;
		flash_write_pending = false;
	} else {
		flash_write_byte = c;
		flash_write_pending = true;
	}

	/* Check for end page */
	if (flash_write_offset % samd21_nvmctrl_page_size() == 0)
		ao_flash_finish_page(__flash8__ + flash_write_offset - samd21_nvmctrl_page_size());
}

void
ao_flash_write_flush(void)
{
	uint32_t	page_size = samd21_nvmctrl_page_size();
	if (flash_write_pending)
		ao_flash_write_byte(0xff);
	if ((flash_write_offset % page_size) != 0)
		ao_flash_finish_page(__flash8__ + (flash_write_offset & ~(page_size - 1)));
}

static uint32_t flash_read_offset;

void
ao_flash_read_init(void)
{
	flash_read_offset =  0;
}

uint8_t
ao_flash_read_byte(void)
{
	if (flash_read_offset >= ao_flash_size())
		return 0xff;
	return __flash8__[flash_read_offset++];
}

static uint32_t
ao_flash_first_erased(void)
{
	uint32_t	offset = ao_flash_size();

	while (offset > 0) {
		--offset;
		if (__flash8__[offset] != 0xff)
			return offset + 1;
	}
	return 0;
}

void
ao_flash_erase_all(void)
{
	uint32_t	offset = ao_flash_first_erased();
	uint32_t	row_size = samd21_nvmctrl_row_size();

	if (offset == 0)
		return;
	/* compute last byte to erase */
	offset -= 1;
	offset &= ~(row_size - 1);
	for (;;) {
		ao_arch_block_interrupts();
		_ao_flash_unlock(__flash8__ + offset);
		_ao_flash_erase_row(__flash8__ + offset);
		ao_arch_release_interrupts();
		if (offset == 0)
			break;
		offset -= row_size;
	}
}
