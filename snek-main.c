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

#include "snek.h"

int
main (int argc, char **argv)
{
	if (argc > 1) {
		freopen(argv[1], "r", stdin);
		snek_file = argv[1];
	} else {
		snek_file = "<stdin>";
		snek_print_vals = true;
	}
	return snek_parse() == snek_parse_success ? 0 : 1;
}
