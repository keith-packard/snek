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

static newt_poly_t
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

static newt_poly_t
newt_builtin_len(newt_poly_t a)
{
	switch (newt_poly_type(a)) {
	case newt_string:
		return newt_float_to_poly(strlen(newt_poly_to_string(a)));
	case newt_list:
		return newt_float_to_poly(newt_poly_to_list(a)->size);
	default:
		return NEWT_ONE;
	}
}

static newt_poly_t
newt_builtin_printn(newt_poly_t a)
{
	if (newt_poly_type(a) == newt_string)
		fputs(newt_poly_to_string(a), stdout);
	else
		newt_poly_print(stdout, a);
	return NEWT_ZERO;
}

static newt_poly_t
newt_builtin_print(newt_poly_t a)
{
	newt_builtin_printn(a);
	putchar('\n');
	return NEWT_ZERO;
}

static newt_poly_t
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

static newt_poly_t
newt_builtin_sys_stdout_flush(void)
{
	fflush(stdout);
	return NEWT_ONE;
}

const newt_builtin_t newt_builtins[] = {
	[NEWT_BUILTIN_exit - 1] {
		.nformal = 1,
		.func1 = newt_builtin_exit,
	},
	[NEWT_BUILTIN_len - 1] {
		.nformal = 1,
		.func1 = newt_builtin_len,
	},
	[NEWT_BUILTIN_print - 1] {
		.nformal = 1,
		.func1 = newt_builtin_print,
	},
	[NEWT_BUILTIN_printn - 1] {
		.nformal = 1,
		.func1 = newt_builtin_printn,
	},
	[NEWT_BUILTIN_time_sleep - 1] {
		.nformal = 1,
		.func1 = newt_builtin_time_sleep,
	},
	[NEWT_BUILTIN_sys_stdout_flush - 1] {
		.nformal = 0,
		.func0 = newt_builtin_sys_stdout_flush,
	},
};

const newt_mem_t newt_builtin_mem = {
	.size = newt_null_size,
	.mark = newt_null_mark,
	.move = newt_null_move
};
