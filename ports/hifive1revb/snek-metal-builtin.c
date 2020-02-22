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

#include <snek.h>
#include <metal/timer.h>
#include <metal/watchdog.h>

static uint32_t
snek_millis(void)
{
	unsigned long long mcc;
	unsigned long long timebase;
	metal_timer_get_cyclecount(0, &mcc);
	metal_timer_get_timebase_frequency(0, &timebase);
	return (uint32_t) ((uint64_t) mcc * 1000 / timebase);
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	uint32_t	expire = snek_millis() + (snek_poly_get_float(a) * 1000.0f + 0.5f);
	while (!snek_abort && (int32_t) (expire - snek_millis()) > 0)
		;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly(snek_millis() / 1000.0f);
}

static uint64_t random_x, random_w;

#define random_s 0xb5ad4eceda1ce2a9ULL

snek_poly_t
snek_builtin_random_seed(snek_poly_t a)
{
	random_x = a.u;
	random_x |= random_x << 32;
	random_w = 0;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	random_x *= random_x;
	random_w += random_s;
	random_x += random_w;
	random_x = (random_x >> 32) | (random_x << 32);
	return snek_float_to_poly(((snek_soffset_t) (random_x % snek_poly_get_soffset(a))));
}

snek_poly_t
snek_builtin_reset(void)
{
	struct metal_watchdog *wdog = metal_watchdog_get_device(0);
	metal_watchdog_set_result(wdog, METAL_WATCHDOG_FULL_RESET);
	metal_watchdog_run(wdog, METAL_WATCHDOG_RUN_ALWAYS);
	metal_watchdog_set_timeout(wdog, 0);
	for (;;) {}
	return SNEK_NULL;
}
