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
#include "snek-windows.h"
#include <getopt.h>
#include <windows.h>

FILE	*snek_windows_input;

static const struct option options[] = {
	{ .name = "version", .has_arg = 0, .val = 'v' },
	{ .name = "help", .has_arg = 0, .val = '?' },
	{ 0 },
};

static void
usage (char *program, int val)
{
	fprintf(stderr, "usage: %s [--version] [--help] <program.py>\n", program);
	exit(val);
}

#define MAX_LINE	4096

static int
snek_getc_interactive(void)
{
	static bool midline;
	int	    c;

	if (!midline)
	{
		char *prompt = "> ";
		if (snek_parse_middle)
			prompt = "+ ";
		fputs(prompt, stdout);
		midline = true;
	}
	c = getchar();
	if (c == '\n')
		midline = false;
	return c;
}

int
snek_getc(FILE *input)
{
	if (snek_interactive)
		return snek_getc_interactive();
	return getc(input);
}

int
main (int argc, char **argv)
{
	int c;

	while ((c = getopt_long(argc, argv, "v?", options, NULL)) != -1) {
		switch (c) {
		case 'v':
			printf("%s version %s\n", argv[0], SNEK_VERSION);
			exit(0);
			break;
		case '?':
			usage(argv[0], 0);
			break;
		default:
			usage(argv[0], 1);
			break;
		}
	}

	if (argv[optind]) {
		snek_file = argv[optind];
		snek_windows_input = fopen(snek_file, "r");
		if (!snek_windows_input) {
			perror(snek_file);
			exit(1);
		}
	} else {
		snek_windows_input = stdin;
		snek_interactive = true;
		printf("Welcome to Snek version %s\n", SNEK_VERSION);
	}

	snek_init();

	bool ret = snek_parse() == snek_parse_success;

	if (snek_windows_input == stdin)
		printf("\n");
	return ret ? 0 : 1;
}

snek_poly_t
snek_builtin_exit(snek_poly_t a)
{
	int ret;
	switch (snek_poly_type(a)) {
	case snek_float:
		ret = (int) snek_poly_to_float(a);
		break;
	default:
		ret = snek_poly_true(a) ? 0 : 1;
		break;
	}
	exit(ret);
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	if (snek_poly_type(a) == snek_float) {
		float delay = snek_poly_to_float(a);
		float ms = floorf(delay * 1000 + 0.5);

		Sleep((DWORD) ms);
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly(GetTickCount() / 1000.0f);
}

snek_poly_t
snek_builtin_random_seed(void)
{
	srand(GetTickCount());
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	return snek_float_to_poly(rand() % (long int) snek_poly_get_float(a));
}

