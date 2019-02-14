/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
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
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

static struct termios termios_save;

snek_poly_t
snek_builtin_curses_initscr(void)
{
	struct termios termios;
	if (tcgetattr(0, &termios) < 0) {
		perror("tcgetattr");
		exit(1);
	}
	termios_save = termios;
	termios.c_lflag &= ~(ICANON|ECHO);
	termios.c_cc[VMIN] = 1;
	termios.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSAFLUSH, &termios) < 0) {
		perror("tcsetattr");
		exit(1);
	}
	printf("\033[?47h");
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_curses_noecho(void)
{
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_curses_echo(void)
{
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_curses_cbreak(void)
{
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_curses_nocbreak(void)
{
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_curses_endwin(void)
{
	printf("\033[?47l");
	tcsetattr(0, TCSAFLUSH, &termios_save);
	return SNEK_ONE;
}

static bool stdscr_nodelay;

snek_poly_t
snek_builtin_stdscr_nodelay(snek_poly_t a)
{
	stdscr_nodelay = snek_poly_true(a);
	return a;
}

snek_poly_t
snek_builtin_stdscr_erase(void)
{
	printf("\033[2J");
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_stdscr_addstr(snek_poly_t y, snek_poly_t x, snek_poly_t str)
{
	snek_builtin_stdscr_move(y, x);
	if (snek_poly_type(str) == snek_string)
		printf("%s", snek_poly_to_string(str));
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_stdscr_move(snek_poly_t y, snek_poly_t x)
{
	printf("\033[%d;%dH", snek_poly_get_soffset(y)+1, snek_poly_get_soffset(x)+1);
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_stdscr_refresh(void)
{
	fflush(stdout);
	return SNEK_ONE;
}

snek_poly_t
snek_builtin_stdscr_getch(void)
{
	struct pollfd pollfd = {
		.fd = 0,
		.events = POLLIN,
		.revents = 0
	};
	int timeout = -1;
	if (stdscr_nodelay)
		timeout = 0;
	int ret = poll(&pollfd, 1, timeout);
	if (ret > 0 && (pollfd.revents & POLLIN)) {
		int val;
		char c;
		val = read(0, &c, 1);
		if (val == 1) {
			return snek_float_to_poly(c);
		}
	}
	if (ret < 0)
		perror("poll");
	return snek_float_to_poly(-1);
}
