/*
 * Copyright © 2021 Keith Packard <keithp@keithp.com>
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

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	int fd;
	char *port = "/dev/ttyACM0";
	struct termios t;
	int DTR_flag;

	if (argv[1])
		port = argv[1];

	fd = open(port,O_RDWR | O_NOCTTY);

	DTR_flag = TIOCM_DTR;
	if (ioctl(fd,TIOCMBIS,&DTR_flag) != 0) {
		perror("dtr");
		exit(1);
	}

	if (tcgetattr(fd, &t) != 0) {
		perror("tcgetattr");
		exit(1);
	}
	cfsetspeed(&t, B1200);
	if (tcsetattr(fd, TCSANOW, &t) != 0) {
		perror("tcsetattr");
		exit(1);
	}

	sleep(1);
	DTR_flag = TIOCM_DTR;
	if (ioctl(fd,TIOCMBIC,&DTR_flag) != 0) {
		perror("dtr");
		exit(1);
	}

	close(fd);
	return 0;
}
