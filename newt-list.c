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

static int
newt_list_size(void *addr)
{
	return 0;
}

static void
newt_list_mark(void *addr)
{
}

static void
newt_list_move(void *addr)
{
}

const newt_mem_t newt_list_mem = {
	.size = newt_list_size,
	.mark = newt_list_mark,
	.move = newt_list_move,
	.name = "list",
};
