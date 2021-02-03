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
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

FILE	*snek_posix_input;

static const struct option options[] = {
	{ .name = "version", .has_arg = 0, .val = 'v' },
	{ .name = "interactive", .has_arg = 0, .val = 'i' },
	{ .name = "help", .has_arg = 0, .val = '?' },
	{ .name = NULL, .has_arg = 0, .val = 0 },
};

static void
usage (char *program, int val)
{
	fprintf(stderr, "usage: %s [--version] [--help] [--interactive] <program.py>\n", program);
	exit(val);
}

static int
snek_getc_interactive(void)
{
	static char	*line, *line_base;
	int	    c;

	if (!line)
	{
		char *prompt = "> ";
		if (snek_parse_middle)
			prompt = "+ ";
		line_base = readline (prompt);
		line = line_base;
		if (!line)
			return EOF;
		add_history (line_base);
	}
	c = (*line++) & 0xff;
	if (!c) {
		c = '\n';
		free (line_base);
		line = 0;
	}
	return c;
}

int
snek_getc(FILE *input)
{
	int c = EOF;
	if (!snek_abort) {
		if (snek_interactive)
			c = snek_getc_interactive();
		else
			c = getc(input);
	}
	if (snek_abort)
		return EOF;
	return c;
}

static void
sigint(int sig)
{
	(void) sig;
	snek_abort = true;
	signal(SIGINT, sigint);
}

int
main (int argc, char **argv)
{
	int c;
	bool do_interactive = true;
	bool interactive_flag = false;

	while ((c = getopt_long(argc, argv, "v?i", options, NULL)) != -1) {
		switch (c) {
		case 'v':
			printf("%s version %s\n", argv[0], SNEK_VERSION);
			exit(0);
			break;
		case 'i':
			interactive_flag = true;
			break;
		case '?':
			usage(argv[0], 0);
			break;
		default:
			usage(argv[0], 1);
			break;
		}
	}

	signal(SIGINT, sigint);

	snek_init();

	bool ret = true;

	if (argv[optind]) {
		snek_file = argv[optind];
		snek_posix_input = fopen(snek_file, "r");
		if (!snek_posix_input) {
			perror(snek_file);
			exit(1);
		}
		if (snek_parse() != snek_parse_success)
			ret = false;
		fclose(snek_posix_input);
		do_interactive = interactive_flag;
	}

	if (do_interactive) {
		printf("Welcome to Snek version %s\n", SNEK_VERSION);
		snek_file = "<stdin>";
		snek_posix_input = stdin;
		snek_interactive = true;
		if (snek_parse() != snek_parse_success)
			ret = false;
		printf("\n");
	}

	return ret ? 0 : 1;
}
