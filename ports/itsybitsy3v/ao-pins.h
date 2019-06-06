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

#define AVR_VCC_5V	       	0
#define AVR_VCC_3V3		1

#define SNEK_IO_GETC(file)	ao_usb_getc()
#define SNEK_GETC()		(snek_avr_file.get(stdin))
#define fflush(x)		ao_usb_flush(x)

#define PIN_MAP		{			\
	[0] = MAKE_MAP(PD, 2),			\
	[1] = MAKE_MAP(PD, 3),			\
	[2] = MAKE_MAP(PD, 1),			\
	[3] = MAKE_MAP(PD, 0),			\
	[4] = MAKE_MAP(PD, 4),			\
	[5] = MAKE_MAP(PC, 6),			\
	[6] = MAKE_MAP(PD, 7),			\
	[7] = MAKE_MAP(PE, 6),			\
	[8] = MAKE_MAP(PB, 4),			\
	[9] = MAKE_MAP(PB, 5),			\
	[10] = MAKE_MAP(PB, 6),			\
	[11] = MAKE_MAP(PB, 7),			\
	[12] = MAKE_MAP(PD, 6),			\
	[13] = MAKE_MAP(PC, 7),			\
	[14] = MAKE_MAP(PF, 7),			\
	[15] = MAKE_MAP(PF, 6),			\
	[16] = MAKE_MAP(PF, 5),			\
	[17] = MAKE_MAP(PF, 4),			\
	[18] = MAKE_MAP(PF, 1),			\
	[19] = MAKE_MAP(PF, 0),			\
	[20] = MAKE_MAP(PB, 3),			\
	[21] = MAKE_MAP(PB, 2),			\
	[22] = MAKE_MAP(PB, 1),			\
	}

#define ADC_MAP		{			\
		[14 - FIRST_ADC] = 7,		\
		[15 - FIRST_ADC] = 6,		\
		[16 - FIRST_ADC] = 5,		\
		[17 - FIRST_ADC] = 4,		\
		[18 - FIRST_ADC] = 1,		\
		[19 - FIRST_ADC] = 0,		\
	}

#define OCR_REG_ADDRS	{				\
		[3] = (uint8_t) (uintptr_t) &OCR0B,	\
		[5] = (uint8_t) (uintptr_t) &OCR3AL,	\
		[6] = (uint8_t) (uintptr_t) &OCR4D,	\
		[9] = (uint8_t) (uintptr_t) &OCR1AL,	\
		[10] = (uint8_t) (uintptr_t) &OCR1BL,	\
		[11] = (uint8_t) (uintptr_t) &OCR0A,	\
		[13] = (uint8_t) (uintptr_t) &OCR4A,	\
	}

#define TCC_REG_ADDRS	{				\
		[3] = (uint8_t) (uintptr_t) &TCCR0A,  	\
		[5] = (uint8_t) (uintptr_t) &TCCR3A,	\
		[6] = (uint8_t) (uintptr_t) &TCCR4A,	\
		[9] = (uint8_t) (uintptr_t) &TCCR1A,	\
		[10] = (uint8_t) (uintptr_t) &TCCR1A,	\
		[11] = (uint8_t) (uintptr_t) &TCCR0A,	\
		[13] = (uint8_t) (uintptr_t) &TCCR4A,	\
	}

#define TCC_REG_VALS	{			\
		[3] = 1 << COM0B1,		\
		[5] = 1 << COM3A1,		\
		[6] = 1 << COM4D1,		\
		[9] = 1 << COM1A1,		\
		[10] = 1 << COM1B1,		\
		[11] = 1 << COM0A1,		\
		[13] = 1 << COM4A1,		\
	}

#endif /* _AO_PINS_H_ */
