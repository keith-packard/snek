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
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _SNEK_METAL_H_
#define _SNEK_METAL_H_

int
snek_uart_getc(FILE *f);

int
snek_uart_putc(char c, FILE *f);

void
snek_uart_setup(void);

#define SNEK_IO_GETC(s) snek_uart_getc(NULL)

void
snek_gpio_init(void);

#endif /* _SNEK_METAL_H_ */
