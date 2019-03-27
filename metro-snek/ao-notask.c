/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
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

#include <ao.h>

static volatile void *ao_wchan;

uint8_t
ao_sleep(void *wchan)
{
#if 1
	ao_wchan = wchan;
	ao_arch_wait_interrupt();
#else
	uint8_t	sreg;

	ao_wchan = wchan;
	asm("in %0,__SREG__" : "=&r" (sreg));
	sei();
	while (ao_wchan)
		ao_arch_cpu_idle();
	asm("out __SREG__,%0" : : "r" (sreg));
#endif
	return 0;
}

void
ao_delay(uint16_t ticks)
{
	AO_TICK_TYPE	target;

	if (!ticks)
		ticks = 1;
	target = ao_tick_count + ticks;
	do {
		ao_sleep(&ao_time);
	} while ((int16_t) (target - ao_tick_count) > 0);
}

void
ao_wakeup(void *wchan)
{
	(void) wchan;
	ao_wchan = 0;
}
