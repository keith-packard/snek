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

#ifndef _AO_PINS_H_
#define _AO_PINS_H_

#define HAS_USB			1
#define HAS_BEEP		0
#define AO_FIFO_SIZE		64

#define AVR_VCC_5V	       	1
#define AVR_VCC_3V3		0

#define SNEK_IO_GETC(file)	ao_usb_getc()
#define SNEK_GETC()		(snek_avr_file.get(stdin))
#define fflush(x)		ao_usb_flush(x)

#endif /* _AO_PINS_H_ */
