/*
 * Copyright © 2020 Keith Packard <keithp@keithp.com>
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

#if defined(__AVR_ATmega4809__)
#define timer_vect 	TCA0_OVF_vect
#define timer_cnt 	TCA0_SINGLE_CNT
#define clear_irq()	(TCA0_SINGLE_INTFLAGS = 1)
#define CLOCK_SHIFT	6
typedef uint16_t TICK_TYPE;
#else
#define timer_vect 	TIMER2_OVF_vect
#define timer_cnt 	TCNT2
#define clear_irq()
#define CLOCK_SHIFT	0
typedef uint8_t TICK_TYPE;
#endif

/*
 * Timer is running at 1/64 F_CPU, and the overflow hits ever 256
 * clocks. We can measure down to the timer resolution by reading
 * the 'tocks' value (number of overflows) and adding in the current
 * timer count
 */

#define TICKS_PER_SECOND	(F_CPU / 64.0f)
#define SECONDS_PER_TICK	(64.0f / F_CPU)

volatile uint32_t timer_tocks = 0;

ISR(timer_vect)
{
	clear_irq();
	timer_tocks++;
}

uint32_t
snek_ticks(void)
{
	uint32_t	tocks_before, tocks_after;
	TICK_TYPE	ticks;

	/* Read the 'tocks' value twice to make sure
	 * we don't hit right across an interrupt
	 */
	do {
		cli();
		tocks_before = timer_tocks;
		sei();
		ticks = timer_cnt;
		cli();
		tocks_after = timer_tocks;
		sei();
	} while (tocks_before != tocks_after);
	return (tocks_before << (8 * sizeof(ticks) - CLOCK_SHIFT)) | (ticks >> CLOCK_SHIFT);
}

static inline uint32_t
snek_sec_to_ticks(float sec)
{
	return (uint32_t) (sec * TICKS_PER_SECOND + 0.5f);
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	uint32_t then = snek_ticks() + snek_sec_to_ticks(snek_poly_get_float(a));

	while (!snek_abort && (int32_t) (then - snek_ticks()) > 0)
	       ;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly((float) snek_ticks() * SECONDS_PER_TICK);
}
