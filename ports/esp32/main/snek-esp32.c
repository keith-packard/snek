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
 */

#include "snek.h"
#include "esp_system.h"

snek_poly_t
snek_builtin_reset(void)
{
	esp_restart();
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	TickType_t ticks;

	ticks = snek_poly_get_float(a) * 1000.0f / portTICK_PERIOD_MS + 0.5f;
	vTaskDelay(ticks);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	TickType_t ticks = xTaskGetTickCount();
	return snek_float_to_poly((float) ticks * (portTICK_PERIOD_MS / 1000.0f));
}
