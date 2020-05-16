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

/*
 * TIMER0 is running at 1/64 F_CPU, and the overflow hits ever 256
 * clocks. We can measure down to the timer resolution by reading
 * the 'tocks' value (number of overflows) and adding in the current
 * timer count
 */

#define TICKS_PER_SECOND	(F_CPU / 64.0f)
#define SECONDS_PER_TICK	(64.0f / F_CPU)

volatile uint32_t timer0_tocks = 0;

ISR(TIMER0_OVF_vect)
{
	timer0_tocks++;
}


static uint32_t
snek_ticks(void)
{
	uint32_t	tocks_before, tocks_after;
	uint8_t		ticks;

	/* Read the 'tocks' value twice to make sure
	 * we don't hit right across an interrupt
	 */
	do {
		cli();
		tocks_before = timer0_tocks;
		sei();
		ticks = TCNT0;
		cli();
		tocks_after = timer0_tocks;
		sei();
	} while (tocks_before != tocks_after);
	return (tocks_before << 8) | ticks;
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
