/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "newt.h"
#include <time.h>

newt_poly_t
newt_builtin_exit(newt_poly_t a)
{
	int ret;
	switch (newt_poly_type(a)) {
	case newt_float:
		ret = (int) newt_poly_to_float(a);
		break;
	default:
		ret = newt_poly_true(a) ? 0 : 1;
		break;
	}
	exit(ret);
}

newt_poly_t
newt_builtin_time_sleep(newt_poly_t a)
{
	if (newt_poly_type(a) == newt_float) {
		float delay = newt_poly_to_float(a);
		float secs = floorf(delay);
		float ns = floorf((delay - secs) * 1e9 + 0.5);

		struct timespec ts = {
			.tv_sec = (time_t) secs,
			.tv_nsec = ns
		};

		nanosleep(&ts, NULL);
	}
	return NEWT_ONE;
}
