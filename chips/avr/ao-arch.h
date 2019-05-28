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

#ifndef _AO_ARCH_H_
#define _AO_ARCH_H_

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

/*
 * AVR definitions and code fragments for AltOS
 */

#ifndef AO_STACK_SIZE
#define AO_STACK_SIZE	116
#endif

/* Various definitions to make GCC look more like SDCC */

#define ao_arch_naked_declare	__attribute__((naked))
#define ao_arch_naked_define
#define __pdata
#define __data
#define __xdata
#define __code const
#define __reentrant
#define __critical
#define __interrupt(n)
#define __at(n)

#define ao_arch_reboot()	/* XXX */

#define ao_arch_nop()		asm("nop")

#define ao_arch_interrupt(n)	/* nothing */

#define AO_ROMCONFIG_VERSION	2

#define AO_ROMCONFIG_SYMBOL(a) const

extern AO_ROMCONFIG_SYMBOL(0) uint16_t ao_serial_number;

#define AVR_PUSH8(stack, val)	(*((stack)--) = (val))

/* Idle the CPU (if possible) waiting for an interrupt. Enabling
 * interrupts and sleeping the CPU must be adjacent to eliminate race
 * conditions. In all cases, we execute a single nop with interrupts
 * enabled
 */

#define ao_arch_wait_interrupt() do {		\
		sleep_enable();			\
		sei();				\
		sleep_cpu();			\
		sleep_disable();		\
		ao_arch_nop();			\
		cli();				\
	} while (0)

#define ao_arch_restore_stack() do { \
		uint8_t	sp_l, sp_h;					\
		sp_l = (uint16_t) ao_cur_task->sp;			\
		sp_h = ((uint16_t) ao_cur_task->sp) >> 8;		\
		asm("out __SP_H__,%0" : : "r" (sp_h) );			\
		asm("out __SP_L__,%0" : : "r" (sp_l) );			\
		asm("pop r0"	"\n\t"					\
		    "out __SREG__, r0");				\
		asm("pop r0" "\n\t" "pop r1" "\n\t" "pop r2" "\n\t" "pop r3" "\n\t" "pop r4"); \
		asm("pop r5" "\n\t" "pop r6" "\n\t" "pop r7" "\n\t" "pop r8" "\n\t" "pop r9"); \
		asm("pop r10" "\n\t" "pop r11" "\n\t" "pop r12" "\n\t" "pop r13" "\n\t" "pop r14"); \
		asm("pop r15" "\n\t" "pop r16" "\n\t" "pop r17" "\n\t" "pop r18" "\n\t" "pop r19"); \
		asm("pop r20" "\n\t" "pop r21" "\n\t" "pop r22" "\n\t" "pop r23" "\n\t" "pop r24"); \
		asm("pop r25" "\n\t" "pop r26" "\n\t" "pop r27" "\n\t" "pop r28" "\n\t" "pop r29"); \
		asm("pop r30" "\n\t" "pop r31");			\
		asm("ret");						\
	} while(0)

#define ao_arch_critical(b) do { cli(); do { b } while (0); sei(); } while (0)

#define ao_arch_block_interrupts()	cli()
#define ao_arch_release_interrupts()	sei()

#define AO_TELESCIENCE_NUM_ADC	12

#endif /* _AO_ARCH_H_ */

