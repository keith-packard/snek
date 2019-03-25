/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

/*
 * Samd21 definitions and code fragments for AltOS
 */

#ifndef AO_STACK_SIZE
#define AO_STACK_SIZE	512
#endif

#define AO_LED_TYPE	uint16_t

#define AO_PORT_TYPE	uint16_t

#define ao_arch_naked_declare	__attribute__((naked))
#define ao_arch_naked_define

#define ao_arch_reboot() \
	(samd21_scb.aircr = ((SAMD21_SCB_AIRCR_VECTKEY_KEY << SAMD21_SCB_AIRCR_VECTKEY) | \
			  (1 << SAMD21_SCB_AIRCR_SYSRESETREQ)))

#define ao_arch_nop()		asm("nop")
#define ao_arch_interrupt(n)	/* nothing */
#define ao_arch_block_interrupts()	asm("cpsid i")
#define ao_arch_release_interrupts()	asm("cpsie i")

/*
 * ao_timer.c
 *
 * For the samd21, we want to use the DFLL48 clock
 */

#ifndef AO_SYSCLK

#if AO_DFLL48M
#define AO_SYSCLK	AO_DFLL48M
#endif

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

extern const uint32_t	ao_radio_cal;

void
ao_adc_init(void);

/* ADC maximum reported value */
#define AO_ADC_MAX			4095

#ifndef HAS_BOOT_LOADER
#define HAS_BOOT_LOADER			1
#endif

#if HAS_BOOT_LOADER
#define AO_BOOT_APPLICATION_BASE	((uint32_t *) 0x00001000)
#ifndef AO_BOOT_APPLICATION_BOUND
#define AO_BOOT_APPLICATION_BOUND	((uint32_t *) (0x00000000 + samd21_flash_size()))
#endif
#define AO_BOOT_LOADER_BASE		((uint32_t *) 0x00000000)
#endif

#endif /* _AO_ARCH_H_ */


