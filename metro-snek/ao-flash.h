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

#ifndef _AO_FLASH_H_
#define _AO_FLASH_H_

void
ao_flash_write_init(void);

void
ao_flash_write_byte(uint8_t c);

void
ao_flash_write_flush(void);

void
ao_flash_read_init(void);

uint8_t
ao_flash_read_byte(void);

void
ao_flash_erase_all(void);

#endif /* _AO_FLASH_H_ */
