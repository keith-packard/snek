/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
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
#include <getopt.h>
#include "sensors.h"
#include "motors.h"

static FILE *snek_posix_input;

static const struct option options[] = {
	{.name = "version", .has_arg = 0, .val = 'v'},
	{.name = "file", .has_arg = 1, .val = 'f'},
	{.name = "help", .has_arg = 0, .val = '?'},
	{.name = NULL, .has_arg = 0, .val = 0},
};

static void
usage(char *program, int val)
{
	fprintf(stderr, "usage: %s [--version] [--help] [--file <file.py>] <program.py>\n", program);
	exit(val);
}

static int
snek_getc_interactive(void)
{
	static char  line_base[4096];
	static char *line;
	int	     c;

	if (!line) {
		char *prompt = "> ";
		if (snek_parse_middle)
			prompt = "+ ";
		fputs(prompt, stdout);
		line = fgets(line_base, 4096, stdin);
		if (!line)
			return EOF;
	}
	c = (*line++) & 0xff;
	if (!c || c == '\n') {
		c = '\n';
		line = 0;
	}
	return c;
}

int
snek_getc(void)
{
	if (snek_interactive)
		return snek_getc_interactive();
	return getc(snek_posix_input);
}

int
main(int argc, char **argv)
{
	int   c;
	char *file = NULL;

	while ((c = getopt_long(argc, argv, "v?f:", options, NULL)) != -1) {
		switch (c) {
		case 'v':
			printf("%s version %s\n", argv[0], SNEK_VERSION);
			exit(0);
			break;
		case 'f':
			file = optarg;
			break;
		case '?':
			usage(argv[0], 0);
			break;
		default:
			usage(argv[0], 1);
			break;
		}
	}

	snek_ev3_init_colors();

	snek_init();

	if (file) {
		snek_file = file;
		snek_posix_input = fopen(snek_file, "r");
		if (!snek_posix_input) {
			perror(snek_file);
			exit(1);
		}
		snek_parse();
	}

	if (argv[optind]) {
		snek_file = argv[optind];
		snek_posix_input = fopen(snek_file, "r");
		if (!snek_posix_input) {
			perror(snek_file);
			exit(1);
		}
	} else {
		snek_file = "<stdin>";
		snek_posix_input = stdin;
		snek_interactive = true;
		printf("Welcome to Snek version %s\n", SNEK_VERSION);
	}

	bool ret = snek_parse() == snek_parse_success;
	if (snek_posix_input == stdin)
		printf("\n");
	return ret ? 0 : 1;
}

snek_poly_t
snek_builtin_read(snek_poly_t port)
{
	switch (snek_poly_type(port)) {
	case snek_float:
		return snek_ev3_sensors_read(port);
	case snek_string:
		return snek_ev3_motors_read(port);
	default:
		snek_error_type_1(port);
		return SNEK_NULL;
	}
}
