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

#ifndef _AO_ARCH_H_
#define _AO_ARCH_H_

#include <stdio.h>
#include <samd21.h>

#define CONST const

/*
 * Samd21 definitions and code fragments for AltOS
 */

#define AO_LED_TYPE	uint16_t

#define ao_arch_naked_declare	__attribute__((naked))
#define ao_arch_naked_define

#define ao_arch_reboot() \
	(samd21_scb.aircr = ((SAMD21_SCB_AIRCR_VECTKEY_KEY << SAMD21_SCB_AIRCR_VECTKEY) | \
			  (1 << SAMD21_SCB_AIRCR_SYSRESETREQ)))

#define ao_arch_nop()		__asm__("nop")
#define ao_arch_interrupt(n)	/* nothing */
#define ao_arch_block_interrupts()	__asm__("cpsid i")
#define ao_arch_release_interrupts()	__asm__("cpsie i")

/*
 * ao_timer.c
 *
 * For the samd21, we want to use the DFLL48 clock
 */

/* GCLK 0 is always the system clock source */

#define AO_GCLK_SYSCLK	0

/* If there's a 32kHz xtal, use that for the 32kHz oscillator */
#ifdef AO_XOSC32K
# ifndef AO_GCLK_XOSC32K
#  define AO_GCLK_XOSC32K	1
# endif
#endif

/* If there's a high-freq xtal, use that */
#ifdef AO_XOSC
# ifndef AO_GCLK_XOSC
#  define AO_GCLK_XOSC	1
# endif

# ifndef AO_XOSC_GCLK_DIV
#  define AO_XOSC_GCLK_DIV 1
# endif

# define AO_FDPLL96M	((AO_XOSC_FREQ / AO_XOSC_DIV * AO_XOSC_MUL) / AO_XOSC_GCLK_DIV)

/* By default, use the xosc for the system, but allow the dfll48m to
 * drive USB if desired.
 */

# ifndef AO_GCLK_FDPLL96M
#  define AO_SYSCLK		AO_FDPLL96M
#  define AO_GCLK_FDPLL96M	AO_GCLK_SYSCLK
#  define AO_GCLK_DFLL48M	2
# endif

#endif	/* AO_XOSC */

#if AO_DFLL48M
# ifndef AO_GCLK_DFLL48M
#  define AO_SYSCLK	AO_DFLL48M
#  define AO_GCLK_DFLL48M AO_GCLK_SYSCLK
# endif
#endif

#ifndef AO_GCLK_USB
# if AO_DFLL48M
#  define AO_GCLK_USB AO_GCLK_DFLL48M
# else
#  define AO_GCLK_USB AO_GCLK_SYSCLK
# endif
#endif

#define AO_HCLK		(AO_SYSCLK / AO_AHB_PRESCALER)

#define AO_HCLK		(AO_SYSCLK / AO_AHB_PRESCALER)
#define AO_PCLK		(AO_HCLK / AO_APB_PRESCALER)
#define AO_SYSTICK	(AO_HCLK)
#define AO_PANIC_DELAY_SCALE  (AO_SYSCLK / 12000000)

#define AO_SAMD21_NVIC_HIGH_PRIORITY	(0 << 6)
#define AO_SAMD21_NVIC_CLOCK_PRIORITY	(1 << 6)
#define AO_SAMD21_NVIC_MED_PRIORITY	(2 << 6)
#define AO_SAMD21_NVIC_LOW_PRIORITY	(3 << 6)

/* ADC maximum reported value */
#define AO_ADC_MAX			4095

/* This has to be 65536 so that TCC and TC match; TC isn't configurable */
#define AO_TCC_PERIOD		65536
#define SNEK_PWM_MAX		(AO_TCC_PERIOD-1)

#define AO_TICK_TYPE		uint32_t
#define AO_TICK_SIGNED		int32_t

bool
ao_usb_waiting(void);

#define AO_CMD_LEN		128
#define AO_STACK_SIZE		2048

#endif /* _AO_ARCH_H_ */


